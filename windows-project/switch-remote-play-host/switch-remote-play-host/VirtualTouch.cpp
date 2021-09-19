#include "VirtualTouch.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>

//#define OUTPUT_TOUCH_DIAGNOSTICS

std::string PointerFlagToStr(POINTER_FLAGS pointerFlags)
{
    auto ss = std::stringstream{};

    auto map = std::unordered_map<POINTER_FLAGS, std::string>{
        { POINTER_FLAG_UPDATE, "pf_UPDATE" },
        { POINTER_FLAG_DOWN, "pf_DOWN" },
        { POINTER_FLAG_INRANGE, "pf_INRANGE" },
        { POINTER_FLAG_INCONTACT, "pf_INCONTACT" },
        { POINTER_FLAG_UP, "pf_UP" }
    };

    for (auto const& pair : map)
    {
        if (pair.first & pointerFlags)
            ss << pair.second << " ";
    }

    return ss.str();
}

std::string CreateDiagnosticOutput(std::vector<POINTER_TOUCH_INFO> const& contacts)
{
    auto ss = std::stringstream{};

    for (auto const& c : contacts)
    {
        ss << c.pointerInfo.pointerId << ":  ";
        ss << "[ " << PointerFlagToStr(c.pointerInfo.pointerFlags) << "] ";
        ss << "\n";
    }

    return ss.str();
}

std::string GetLastErrorAsString()
{
    //Get the error message ID, if any.
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0) {
        return std::string(); //No error message has been recorded
    }

    LPSTR messageBuffer = nullptr;

    //Ask Win32 to give us the string version of that message ID.
    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    //Copy the error message into a std::string.
    std::string message(messageBuffer, size);

    //Free the Win32's string's buffer.
    LocalFree(messageBuffer);

    return message;
}

long constexpr magSqr(int x, int y) { return x * x + y * y; }

VirtualTouch::VirtualTouch(int deadzone, int radius)
    : deadzoneRadius{ deadzone }, deadzoneMagSqr{ magSqr(deadzone, deadzone) },
    radius{ radius }, contacts{}, released{}
{
}

void VirtualTouch::Press()
{
    std::vector<VirtualFinger> dummy = { VirtualFinger{} };
    dummy[0].id = 0;

    Press(dummy);
}

void VirtualTouch::Move(int x, int y)
{
    std::vector<VirtualFinger> dummy = { VirtualFinger{} };
    dummy[0].id = 0;
    dummy[0].x = x;
    dummy[0].y = y;

    Move(dummy);
}

void VirtualTouch::Press(std::vector<VirtualFinger> const fingers)
{
    // find all fingers that were lost
    TrackMissing(fingers);

    // update finger states
        // if the finger exists, update it
        // if the finger is new, create its state
    for (auto const& finger : fingers)
    {
        auto itr = contacts.find(finger.id);
        if (itr != contacts.end())
            PressContact(itr->second);
        else
        {
            auto contact = CreateContact(finger.id);
            PressContact(contact);
            contacts[finger.id] = contact;
        }
    }
}

void VirtualTouch::Move(std::vector<VirtualFinger> const fingers)
{
    //make sure to remove missing fingers
    TrackMissing(fingers);

    for (auto const& finger : fingers)
    {
        auto itr = contacts.find(finger.id);
        if (itr != contacts.end())
            MoveContact(itr->second, finger.x, finger.y);
        else
        {
            auto contact = CreateContact(finger.id);
            MoveContact(contact, finger.x, finger.y);
            contacts[finger.id] = contact;
        }
    }
}

void VirtualTouch::Release()
{
    /*auto& pointer = singleContact.pointerInfo;
    auto ignoreFlags = (POINTER_FLAG_NONE | POINTER_FLAG_UP);
    auto flag = pointer.pointerFlags;
    if (flag != POINTER_FLAG_NONE)
        pointer.pointerFlags = POINTER_FLAG_UP;
    else if (flag & POINTER_FLAG_UP)
        pointer.pointerFlags = POINTER_FLAG_NONE;*/

    for (auto& finger : contacts)
        ReleaseContact(finger.second);
}

void VirtualTouch::Commit()
{
    //InjectTouchInput(1, &singleContact);

    //process lost fingers
    if (released.size() != 0)
    {
        auto lostFingers = std::vector<POINTER_TOUCH_INFO>();
        for (auto const& finger : released)
            lostFingers.push_back(finger.second);

        auto releaseLostEvents = CreateReleaseEvents(lostFingers);
        auto failed = InjectTouchInput(releaseLostEvents.size(), releaseLostEvents.data()) == 0;
#if OUTPUT_TOUCH_DIAGNOSTICS
        if (failed)
        {
            std::cout << "Injection failed for release events\n";
            auto err = GetLastErrorAsString();
            std::cout << "Error:\n\t" << err << "\n";
            std::cout << "Contact info: " << CreateDiagnosticOutput(releaseLostEvents) << "\n";
        }
#endif
        released.clear();
    }

    //process tracked fingers
    if (contacts.size() > 0)
    {
        auto injections = std::vector<POINTER_TOUCH_INFO>();
        injections.reserve(contacts.size());
        for (auto const& finger : contacts)
            injections.push_back(finger.second);

        auto failed = InjectTouchInput(contacts.size(), injections.data()) == 0;
#if OUTPUT_TOUCH_DIAGNOSTICS
        if (failed)
        {
            std::cout << "Injection failed for contact events\n";
            auto err = GetLastErrorAsString();
            std::cout << "Error:\n\t" << err << "\n";
            std::cout << "Contact info: " << CreateDiagnosticOutput(injections) << "\n";
        }
#endif
        auto toDelete = std::vector<uint32_t>();
        for (auto const& contact : contacts)
        {
            if (contact.second.pointerInfo.pointerFlags == POINTER_FLAG_UP)
                toDelete.push_back(contact.first);
        }

        for (auto const& id : toDelete)
            contacts.erase(id);
    }
}

bool VirtualTouch::OutsideDeadzone(int x1, int y1, int x2, int y2)
{
    auto dx = x1 - x2;
    auto dy = y1 - y2;
    auto magnitude = magSqr(dx, dy);

    return magnitude > deadzoneMagSqr;
}

POINTER_TOUCH_INFO VirtualTouch::CreateContact(uint32_t id)
{
    auto contact = POINTER_TOUCH_INFO();

    contact.touchFlags = TOUCH_FLAG_NONE;
    contact.touchMask = TOUCH_MASK_CONTACTAREA;

    auto& pointer = contact.pointerInfo;
    pointer.pointerType = PT_TOUCH;
    pointer.pointerId = id;

    return contact;
}

void VirtualTouch::TrackMissing(std::vector<VirtualFinger> const& fingers)
{
    /*auto predicate = [&](std::pair<uint32_t, POINTER_TOUCH_INFO> const& a) {
        auto fingerMatch = [&](VirtualFinger const& finger) {
            return finger.id == a.first;
        };
        auto match = std::find_if(fingers.begin(), fingers.end(), fingerMatch);
        return match != fingers.end();
    };
    auto itr = std::remove_if(contacts.begin(), contacts.end(), predicate);

    for(auto copy = itr; copy != contacts.end(); ++copy)
        released[copy->first] = copy->second;

    contacts.erase(itr, contacts.end());*/
    auto notFound = [&](std::pair<uint32_t, POINTER_TOUCH_INFO> const& a) {
        auto fingerMatch = [&](VirtualFinger const& finger) {
            return finger.id == a.first;
        };
        auto match = std::find_if(fingers.begin(), fingers.end(), fingerMatch);
        return match == fingers.end();
    };

    for (auto const& contact : contacts)
    {
        if (notFound(contact))
            released[contact.first] = contact.second;
    }

    for (auto const& toDelete : released)
        contacts.erase(toDelete.first);
}

void VirtualTouch::PressContact(POINTER_TOUCH_INFO& contact)
{
    auto& pointer = contact.pointerInfo;
    auto contactFlags = (POINTER_FLAG_DOWN | POINTER_FLAG_UPDATE);
    if (!(pointer.pointerFlags & contactFlags))
        pointer.pointerFlags = POINTER_FLAG_DOWN | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;
    else if (pointer.pointerFlags & POINTER_FLAG_DOWN)
        pointer.pointerFlags = POINTER_FLAG_UPDATE | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;
}

void VirtualTouch::MoveContact(POINTER_TOUCH_INFO& contact, int x, int y)
{
    auto& pointer = contact.pointerInfo;
    auto x1 = pointer.ptPixelLocation.x;
    auto y1 = pointer.ptPixelLocation.y;

    if (OutsideDeadzone(x1, y1, x, y))
    {
        pointer.ptPixelLocation.x = x;
        pointer.ptPixelLocation.y = y;

        contact.rcContact.top = y - radius;
        contact.rcContact.bottom = y + radius;
        contact.rcContact.left = x - radius;
        contact.rcContact.right = x + radius;
    }
}

void VirtualTouch::ReleaseContact(POINTER_TOUCH_INFO& contact)
{
    auto& pointer = contact.pointerInfo;
    auto ignoreFlags = (POINTER_FLAG_NONE | POINTER_FLAG_UP);
    auto flag = pointer.pointerFlags;
    pointer.pointerFlags = POINTER_FLAG_UP;
    /*if (flag != POINTER_FLAG_UP && flag != POINTER_FLAG_NONE)
        pointer.pointerFlags = POINTER_FLAG_UP;
    else if (flag & POINTER_FLAG_UP)
        pointer.pointerFlags = POINTER_FLAG_NONE;*/
}

std::vector<POINTER_TOUCH_INFO> VirtualTouch::CreateReleaseEvents(std::vector<POINTER_TOUCH_INFO> const& contacts)
{
    auto events = std::vector<POINTER_TOUCH_INFO>();

    for (auto contact : contacts)
    {
        contact.pointerInfo.pointerFlags = POINTER_FLAG_UP | POINTER_FLAG_CANCELED;
        events.push_back(contact);
        /*while (contact.pointerInfo.pointerFlags != POINTER_FLAG_UP)
        {
            ReleaseContact(contact);
            events.push_back(contact);
        }*/
    }

    return events;
}

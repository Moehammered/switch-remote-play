#include "VirtualTouch.h"
#include <algorithm>
#include "DeadzoneUtility.h"

//#define OUTPUT_TOUCH_DIAGNOSTICS
#ifdef OUTPUT_TOUCH_DIAGNOSTICS
#include "SystemCalls.h"
#include <iostream>
#include <string>
#include <sstream>
#endif

VirtualTouch::VirtualTouch(int deadzone, int radius)
    : deadzoneRadius{ deadzone }, 
    deadzoneMagSqr{ (long)magnitudeSqr(deadzone, deadzone) },
    radius{ radius }, 
    contacts{}, 
    released{}
{
}

void VirtualTouch::Update(std::vector<VirtualFinger> const fingers)
{
    TrackMissing(fingers);

    for (auto const& finger : fingers)
    {
        auto itr = contacts.find(finger.id);
        if (itr == contacts.end())
            contacts[finger.id] = CreateContact(finger.id);

        auto& contact = contacts[finger.id];
        PressContact(contact);
        MoveContact(contact, finger.x, finger.y);
    }
}

void VirtualTouch::Release()
{
    for (auto& finger : contacts)
        ReleaseContact(finger.second);
}

void VirtualTouch::Commit()
{
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

    if (outsideDeadzoneSqr(deadzoneMagSqr, x1, y1, x, y))
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
}

std::vector<POINTER_TOUCH_INFO> VirtualTouch::CreateReleaseEvents(std::vector<POINTER_TOUCH_INFO> const& contacts)
{
    auto events = std::vector<POINTER_TOUCH_INFO>();

    for (auto contact : contacts)
    {
        contact.pointerInfo.pointerFlags = POINTER_FLAG_UP | POINTER_FLAG_CANCELED;
        events.push_back(contact);
    }

    return events;
}

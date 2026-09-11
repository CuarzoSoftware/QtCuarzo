#ifndef COMMON_H
#define COMMON_H

#include <qpa/qplatformmenu.h>
#include <QIcon>
#include <QString>
#include <memory>

#include <CZ/Heaven/Client/HNClient.h>
#include <CZ/Heaven/Client/HNTopbar.h>
#include <CZ/Heaven/Client/HNMenu.h>
#include <CZ/Heaven/Client/HNAction.h>
#include <CZ/Heaven/Client/HNToggle.h>
#include <CZ/Heaven/Client/HNDivider.h>
#include <CZ/Heaven/Client/HNWithParent.h>
#include <CZ/Heaven/Client/HNWithChildren.h>

using namespace CZ;

// The Qt platform classes below are named HN* (HNMenuBar/HNMenu/HNMenuItem) and the Heaven client
// classes live in CZ::HNClientAPI. Both define a class called HNMenu, so instead of pulling the whole
// namespace in (which would make HNMenu ambiguous) we import the Heaven types individually and expose
// the Heaven menu under a distinct alias.
using CZ::HNClientAPI::HNObject;
using CZ::HNClientAPI::HNClient;
using CZ::HNClientAPI::HNTopbar;
using CZ::HNClientAPI::HNAction;
using CZ::HNClientAPI::HNToggle;
using CZ::HNClientAPI::HNDivider;
using CZ::HNClientAPI::HNWithParent;
using CZ::HNClientAPI::HNWithChildren;
using CZ::HNClientAPI::HNWithTitle;
using CZ::HNClientAPI::HNWithIcon;
using CZ::HNClientAPI::HNWithShortcut;
using CZ::HNClientAPI::HNWithEnabled;
using HNHeavenMenu = CZ::HNClientAPI::HNMenu;

class HNMenuBar;
class HNMenu;
class HNMenuItem;

/**
 * @brief Shared Heaven client, handed to every platform object.
 */
struct HNData
{
    std::shared_ptr<HNClient> heaven;

    /// Flush all pending Heaven changes to the bar.
    void commit() const noexcept { if (heaven) heaven->commit(); }
};

/**
 * @brief One Heaven menu object per placement of a Qt menu.
 *
 * A single Qt QPlatformMenu can appear in more than one place at once (several menu bars, or reused
 * as a submenu in different parents). Heaven objects have a single parent, so each placement is a
 * separate Heaven object — a "clone".
 */
struct MenuClone
{
    std::shared_ptr<HNHeavenMenu> object; ///< The Heaven menu (Type::Menu).
    HNMenuBar *bar  = nullptr;      ///< Owning menu bar, if placed directly in a topbar.
    HNMenu    *menu = nullptr;      ///< Owning Qt menu, if nested as a submenu.
};

/**
 * @brief One Heaven object per placement of a Qt menu item (action / toggle / divider).
 */
struct ItemClone
{
    std::shared_ptr<HNObject> object;  ///< The Heaven action / toggle / divider.
    MenuClone *menu    = nullptr;      ///< The menu placement this item lives in.
    MenuClone *subMenu = nullptr;      ///< The submenu placement, if this item opens a submenu.
};

// --- parent helpers (setParent/insertBefore live on HNWithParent, not HNObject) ---

inline void hnSetParent(HNObject *object, HNObject *parent) noexcept
{
    if (auto *wp = dynamic_cast<HNWithParent*>(object))
        wp->setParent(parent);
}

inline void hnInsertBefore(HNObject *object, HNObject *sibling) noexcept
{
    if (auto *wp = dynamic_cast<HNWithParent*>(object))
        wp->insertBefore(sibling);
}

// --- helpers (defined in main.cpp) ---

/// @brief The Heaven parent of @p object, or nullptr.
HNObject *hnParent(HNObject *object) noexcept;

/// @brief The next sibling of @p object within its parent, or nullptr.
HNObject *hnNextSibling(HNObject *object) noexcept;

/// @brief The freedesktop icon name of @p icon (empty if it is not a themed icon).
std::string hnIconName(const QIcon &icon) noexcept;

/// @brief Whether @p object (or one of its ancestors) is represented by @p menu (cycle guard).
bool loopedMenu(HNMenu *menu, HNObject *object);

/// @brief Destroys @p object and every descendant, unlinking their clones from the Qt objects.
void destroyObjectAndChildren(std::shared_ptr<HNObject> object);

#endif // COMMON_H

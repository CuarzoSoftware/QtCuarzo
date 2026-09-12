#include "HNMenu.h"
#include "HNMenuItem.h"
#include "PlatformTheme.h"

HNMenu::HNMenu(HNData *data) : QPlatformMenu(), m_data(data)
{
}

static ItemClone *findItemClone(HNMenuItem *item, MenuClone *menu)
{
    for (ItemClone *clone : item->clones)
        if (clone->menu == menu)
            return clone;

    return nullptr;
}

// The Heaven object an item clone is actually placed in the tree with: the submenu (a menu) when the
// item opens one, otherwise the action/toggle/divider itself.
static HNObject *placedObject(ItemClone *clone)
{
    if (!clone)
        return nullptr;
    return clone->subMenu ? clone->subMenu->object.get() : clone->object.get();
}

HNMenu::~HNMenu()
{
    if (action)
        action->menu = nullptr;

    while (!clones.empty())
    {
        MenuClone *mClone = clones.last();

        for (HNMenuItem *child : children)
        {
            ItemClone *iClone = findItemClone(child, mClone);

            if (iClone)
            {
                child->clones.removeOne(iClone);
                if (iClone->subMenu)
                    destroyObjectAndChildren(iClone->subMenu->object);
                hnUnlinkSubMenu(iClone);
                iClone->object.reset();
                delete iClone;
            }
        }

        hnUnlinkSubMenu(mClone);
        mClone->object.reset();
        clones.removeLast();
        delete mClone;
    }

    m_data->commit();
}

void HNMenu::insertMenuItem(QPlatformMenuItem *menuItem, QPlatformMenuItem *before)
{
    HNMenuItem *i = static_cast<HNMenuItem*>(menuItem);
    HNMenuItem *b = static_cast<HNMenuItem*>(before);

    children.removeOne(i);

    if (b)
        children.insert(children.indexOf(b), i);
    else
        children.append(i);

    for (MenuClone *mClone : clones)
    {
        if (loopedMenu(this, hnParent(mClone->object.get())))
            continue;

        ItemClone *iClone = findItemClone(i, mClone);

        if (!iClone)
        {
            iClone = i->createClone();
            iClone->menu = mClone;

            if (i->menu)
            {
                iClone->subMenu = i->menu->createClone(mClone->object.get());
                iClone->subMenu->parentItem = iClone;
            }
        }

        HNObject *bObject = b ? placedObject(findItemClone(b, mClone)) : nullptr;
        HNObject *target = placedObject(iClone);

        if (bObject)
            hnInsertBefore(target, bObject);
        else
            hnSetParent(target, mClone->object.get());
    }

    m_data->commit();
}

void HNMenu::removeMenuItem(QPlatformMenuItem *menuItem)
{
    HNMenuItem *item = static_cast<HNMenuItem*>(menuItem);

    children.removeOne(item);

    for (MenuClone *mClone : clones)
    {
        ItemClone *iClone = findItemClone(item, mClone);

        if (!iClone)
            continue;

        item->clones.removeOne(iClone);

        if (iClone->subMenu)
            destroyObjectAndChildren(iClone->subMenu->object);

        hnUnlinkSubMenu(iClone);
        iClone->object.reset();
        delete iClone;
    }

    m_data->commit();
}

void HNMenu::syncMenuItem(QPlatformMenuItem *)
{
}

void HNMenu::syncSeparatorsCollapsible(bool)
{
}

void HNMenu::setText(const QString &text)
{
    m_text = text;
    m_text = m_text.remove("&").trimmed();

    if (m_text.isEmpty())
        return;

    for (MenuClone *clone : clones)
        clone->object->setTitle(m_text.toStdString());

    m_data->commit();
}

void HNMenu::setIcon(const QIcon &icon)
{
    m_icon = hnIconName(icon);

    for (MenuClone *clone : clones)
    {
        clone->object->setIcon(m_icon);
        clone->object->setIconFlat(false);
    }

    m_data->commit();
}

void HNMenu::setEnabled(bool enabled)
{
    m_enabled = enabled;

    if (action)
    {
        for (ItemClone *clone : action->clones)
            if (auto *e = dynamic_cast<HNWithEnabled*>(clone->object.get()))
                e->setEnabled(enabled);
    }

    for (MenuClone *clone : clones)
        clone->object->setEnabled(enabled);

    m_data->commit();
}

void HNMenu::setVisible(bool visible)
{
    m_visible = visible;
}

QPlatformMenuItem *HNMenu::menuItemAt(int position) const
{
    if (position >= 0 && position < children.count())
        return static_cast<QPlatformMenuItem*>(children.at(position));

    return nullptr;
}

QPlatformMenuItem *HNMenu::menuItemForTag(quintptr tag) const
{
    for (HNMenuItem *item : children)
        if (item->tag() == tag)
            return item;

    return nullptr;
}

QPlatformMenuItem *HNMenu::createMenuItem() const
{
    return new HNMenuItem(m_data);
}

QPlatformMenu *HNMenu::createSubMenu() const
{
    return new HNMenu(m_data);
}

void HNMenu::setTag(quintptr tag)
{
    m_tag = tag;
}

quintptr HNMenu::tag() const
{
    return m_tag;
}

MenuClone *HNMenu::createClone(HNObject *parent)
{
    MenuClone *clone = new MenuClone;
    clone->object = HNHeavenMenu::Make();
    clone->object->userData = this;

    if (action)
    {
        clone->object->setTitle(action->m_text.toStdString());
        clone->object->setEnabled(action->m_enabled);
    }
    else
    {
        clone->object->setTitle(m_text.toStdString());
        clone->object->setEnabled(m_enabled);
    }

    clones.append(clone);

    if (!loopedMenu(this, parent))
    {
        for (HNMenuItem *child : children)
        {
            MenuClone *mClone = nullptr;

            if (child->menu)
            {
                mClone = child->menu->createClone(clone->object.get());
                mClone->menu = this;
                mClone->object->setParent(clone->object.get());

                if (child->m_text.isEmpty())
                    mClone->object->setTitle(child->menu->m_text.toStdString());
                else
                    mClone->object->setTitle(child->m_text.toStdString());

                if (!child->m_icon.empty())
                {
                    mClone->object->setIcon(child->m_icon);
                    mClone->object->setIconFlat(false);
                }
            }

            ItemClone *iClone = child->createClone();
            iClone->menu = clone;
            iClone->subMenu = mClone;

            if (mClone)
                mClone->parentItem = iClone;

            if (child->menu)
                continue;

            hnSetParent(iClone->object.get(), clone->object.get());
        }
    }

    return clone;
}

std::shared_ptr<HNClient> HNMenu::heaven() const
{
    return m_data->heaven;
}

#include "HNMenuItem.h"
#include "HNMenu.h"

QT_BEGIN_NAMESPACE

// Builds the Heaven object of the type currently implied by the item's flags, with all its
// properties applied and (for actions/toggles) the click signal wired to Qt's activated().
static std::shared_ptr<HNObject> makeItemObject(HNMenuItem *item)
{
    std::shared_ptr<HNObject> obj;

    if (item->m_isSeparator)
    {
        obj = HNDivider::Make(item->m_text.toStdString());
    }
    else if (item->m_isCheckable)
    {
        obj = HNToggle::Make(item->m_text.toStdString(), item->m_icon,
                             item->m_shortcut.toStdString(), item->m_checked, item->m_enabled);
    }
    else
    {
        obj = HNAction::Make(item->m_text.toStdString(), item->m_icon,
                             item->m_shortcut.toStdString(), item->m_enabled);
    }

    if (!obj)
        return obj;

    obj->userData = item;

    if (obj->type() != HNObject::Divider)
    {
        obj->onClicked.subscribe(obj.get(), [item](HNObject*){ emit item->activated(); });

        if (!item->m_icon.empty())
            if (auto *ic = dynamic_cast<HNWithIcon*>(obj.get()))
                ic->setIconFlat(false);
    }

    return obj;
}

// Swaps every (non-submenu) clone's Heaven object for a freshly built one of the correct type,
// re-inserting it at the same position. Used when the item's type changes (separator/checkable).
static void recreateItemClones(HNMenuItem *item)
{
    for (ItemClone *iClone : item->clones)
    {
        if (iClone->subMenu)
            continue; // placed as a submenu; the hidden object is not in the tree

        HNObject *parent = hnParent(iClone->object.get());
        HNObject *before = hnNextSibling(iClone->object.get());

        iClone->object.reset();
        iClone->object = makeItemObject(item);

        if (before)
            hnInsertBefore(iClone->object.get(), before);
        else if (parent)
            hnSetParent(iClone->object.get(), parent);
    }
}

HNMenuItem::HNMenuItem(HNData *data) : QPlatformMenuItem(), m_data(data)
{
}

HNMenuItem::~HNMenuItem()
{
    if (menu)
        menu->action = nullptr;

    while (!clones.empty())
    {
        ItemClone *iClone = clones.last();

        if (iClone->subMenu)
            destroyObjectAndChildren(iClone->subMenu->object);

        iClone->object.reset();
        clones.removeLast();
        delete iClone;
    }

    m_data->commit();
}

void HNMenuItem::setText(const QString &text)
{
    m_text = text;
    m_text = m_text.remove("&");
    const std::string t = m_text.toStdString();

    for (ItemClone *iClone : clones)
    {
        if (iClone->subMenu)
        {
            iClone->subMenu->object->setTitle(t);
            continue;
        }

        if (auto *wt = dynamic_cast<HNWithTitle*>(iClone->object.get()))
            wt->setTitle(t);
    }

    m_data->commit();
}

void HNMenuItem::setIcon(const QIcon &icon)
{
    m_icon = hnIconName(icon);

    for (ItemClone *iClone : clones)
    {
        if (iClone->subMenu)
        {
            iClone->subMenu->object->setIcon(m_icon);
            iClone->subMenu->object->setIconFlat(false);
        }

        if (auto *ic = dynamic_cast<HNWithIcon*>(iClone->object.get()))
        {
            ic->setIcon(m_icon);
            ic->setIconFlat(false);
        }
    }

    m_data->commit();
}

void HNMenuItem::setMenu(QPlatformMenu *mm)
{
    HNMenu *m = static_cast<HNMenu*>(mm);

    for (ItemClone *iClone : clones)
    {
        if (m)
        {
            HNObject *parent = hnParent(iClone->object.get());
            HNObject *before = hnNextSibling(iClone->object.get());

            hnSetParent(iClone->object.get(), nullptr); // remove the plain item from the tree

            iClone->subMenu = m->createClone(parent);

            if (before)
                iClone->subMenu->object->insertBefore(before);
            else if (parent)
                iClone->subMenu->object->setParent(parent);

            iClone->subMenu->object->setTitle(m_text.toStdString());

            if (!m_icon.empty())
            {
                iClone->subMenu->object->setIcon(m_icon);
                iClone->subMenu->object->setIconFlat(false);
            }
        }
        else if (menu && iClone->subMenu)
        {
            HNObject *parent = hnParent(iClone->subMenu->object.get());
            HNObject *before = hnNextSibling(iClone->subMenu->object.get());

            destroyObjectAndChildren(iClone->subMenu->object);
            iClone->subMenu = nullptr;

            if (before)
                hnInsertBefore(iClone->object.get(), before);
            else if (parent)
                hnSetParent(iClone->object.get(), parent);
        }
    }

    if (menu)
        menu->action = nullptr;

    menu = m;

    if (menu)
        menu->action = this;

    m_data->commit();
}

void HNMenuItem::setVisible(bool isVisible)
{
    m_visible = isVisible;
}

void HNMenuItem::setIsSeparator(bool isSeparator)
{
    if (m_isSeparator == isSeparator)
        return;

    m_isSeparator = isSeparator;
    recreateItemClones(this);
    m_data->commit();
}

void HNMenuItem::setRole(MenuRole)
{
}

#if QT_CONFIG(shortcut)
void HNMenuItem::setShortcut(const QKeySequence &shortcut)
{
    m_shortcut = shortcut.toString();
    m_shortcut = m_shortcut.remove(" ");
    const std::string s = m_shortcut.toStdString();

    for (ItemClone *iClone : clones)
        if (auto *ws = dynamic_cast<HNWithShortcut*>(iClone->object.get()))
            ws->setShortcut(s);

    m_data->commit();
}
#endif

void HNMenuItem::setCheckable(bool checkable)
{
    if (m_isCheckable == checkable)
        return;

    m_isCheckable = checkable;
    recreateItemClones(this);
    m_data->commit();
}

void HNMenuItem::setChecked(bool isChecked)
{
    m_checked = isChecked;

    for (ItemClone *iClone : clones)
        if (auto *t = dynamic_cast<HNToggle*>(iClone->object.get()))
            t->setChecked(isChecked);

    m_data->commit();
}

void HNMenuItem::setEnabled(bool enabled)
{
    m_enabled = enabled;

    if (menu)
        menu->setEnabled(enabled);

    for (ItemClone *iClone : clones)
        if (auto *e = dynamic_cast<HNWithEnabled*>(iClone->object.get()))
            e->setEnabled(enabled);

    m_data->commit();
}

void HNMenuItem::setIconSize(int)
{
}

void HNMenuItem::setTag(quintptr tag)
{
    m_tag = tag;
}

quintptr HNMenuItem::tag() const
{
    return m_tag;
}

ItemClone *HNMenuItem::createClone()
{
    ItemClone *clone = new ItemClone;
    clone->object = makeItemObject(this);
    clones.append(clone);
    return clone;
}

std::shared_ptr<HNClient> HNMenuItem::heaven() const
{
    return m_data->heaven;
}

QT_END_NAMESPACE

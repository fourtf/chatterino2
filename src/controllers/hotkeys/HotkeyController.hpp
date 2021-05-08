#pragma once
#include <pajlada/signals/signal.hpp>
#include <pajlada/signals/signalholder.hpp>

#include "common/SignalVector.hpp"
#include "common/Singleton.hpp"
#include "controllers/hotkeys/HotkeyScope.hpp"

namespace chatterino {
class Hotkey;
class Settings;
class Paths;

class HotkeyModel;

class HotkeyController final : public Singleton
{
public:
    HotkeyController();
    HotkeyModel *createModel(QObject *parent);

    virtual void initialize(Settings &settings, Paths &paths) override;
    std::vector<QShortcut *> shortcutsForScope(
        HotkeyScope scope,
        std::map<QString, std::function<QString(std::vector<QString>)>>
            actionMap,
        QWidget *parent);

    void save() override;
    std::shared_ptr<Hotkey> getHotkeyByName(QString name);
    void replaceHotkey(QString oldName, std::shared_ptr<Hotkey> newHotkey);
    boost::optional<HotkeyScope> hotkeyScopeFromName(QString scopeName);
    QString hotkeyScopeToName(HotkeyScope scope);
    bool isDuplicate(std::shared_ptr<Hotkey> hotkey);

    const std::map<HotkeyScope, QString> hotkeyScopeNames = {
        {HotkeyScope::EmotePopup, "emotePopup"},
        {HotkeyScope::SelectChannelPopup, "selectChannelPopup"},
        {HotkeyScope::Settings, "settings"},
        {HotkeyScope::Split, "split"},
        {HotkeyScope::SplitInput, "splitInput"},
        {HotkeyScope::Tab, "tab"},
        {HotkeyScope::UserCard, "userCard"},
        {HotkeyScope::Window, "window"}};

    const std::map<HotkeyScope, QString> hotkeyScopeDisplayNames = {
        {HotkeyScope::EmotePopup, "Emote popup"},
        {HotkeyScope::SelectChannelPopup, "Select channel popup"},
        {HotkeyScope::Settings, "Settings dialog"},
        {HotkeyScope::Split, "Split"},
        {HotkeyScope::SplitInput, "Split input box"},
        {HotkeyScope::Tab, "Tab"},
        {HotkeyScope::UserCard, "User card"},
        {HotkeyScope::Window, "Window"},
    };
    pajlada::Signals::NoArgSignal onItemsUpdated;

private:
    SignalVector<std::shared_ptr<Hotkey>> hotkeys_;
    pajlada::Signals::SignalHolder signalHolder_;

    void loadHotkeys();
    void saveHotkeys();
    void resetToDefaults(std::set<QString> &addedHotkeys);
    void tryAddDefault(std::set<QString> &addedHotkeys, HotkeyScope scope,
                       QKeySequence keySequence, QString action,
                       std::vector<QString> args, QString name);
    void showHotkeyError(std::shared_ptr<Hotkey> hotkey, QString warning);
    friend class KeyboardSettingsPage;
};

}  // namespace chatterino

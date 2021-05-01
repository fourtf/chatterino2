#pragma once

#include "common/Aliases.hpp"
#include "common/Atomic.hpp"
#include "common/UniqueAccess.hpp"
#include "controllers/accounts/Account.hpp"
#include "messages/Emote.hpp"
#include "providers/twitch/TwitchUser.hpp"

#include <rapidjson/document.h>
#include <QColor>
#include <QString>

#include <functional>
#include <mutex>
#include <set>

namespace chatterino {

enum FollowResult {
    FollowResult_Following,
    FollowResult_NotFollowing,
    FollowResult_Failed,
};

struct TwitchEmoteSetResolverResponse {
    const QString channelName;
    const QString channelId;
    const QString type;
    const int tier;
    const bool isCustom;
    // Example response:
    //    {
    //      "channel_name": "zneix",
    //      "channel_id": "99631238",
    //      "type": "",
    //      "tier": 1,
    //      "custom": false
    //    }

    TwitchEmoteSetResolverResponse(QJsonObject jsonObject)
        : channelName(jsonObject.value("channel_name").toString())
        , channelId(jsonObject.value("channel_id").toString())
        , type(jsonObject.value("type").toString())
        , tier(jsonObject.value("tier").toInt())
        , isCustom(jsonObject.value("custom").toBool())
    {
    }
};

class TwitchAccount : public Account
{
public:
    struct TwitchEmote {
        EmoteId id;
        EmoteName name;
    };

    struct EmoteSet {
        QString key;
        QString channelName;
        QString text;
        QString type;
        std::vector<TwitchEmote> emotes;
    };

    std::map<QString, EmoteSet> staticEmoteSets;

    struct TwitchAccountEmoteData {
        std::vector<std::shared_ptr<EmoteSet>> emoteSets;

        std::vector<EmoteName> allEmoteNames;

        EmoteMap emotes;
    };

    TwitchAccount(const QString &username, const QString &oauthToken_,
                  const QString &oauthClient_, const QString &_userID);

    virtual QString toString() const override;

    const QString &getUserName() const;
    const QString &getOAuthToken() const;
    const QString &getOAuthClient() const;
    const QString &getUserId() const;

    QColor color();
    void setColor(QColor color);

    // Attempts to update the users OAuth Client ID
    // Returns true if the value has changed, otherwise false
    bool setOAuthClient(const QString &newClientID);

    // Attempts to update the users OAuth Token
    // Returns true if the value has changed, otherwise false
    bool setOAuthToken(const QString &newOAuthToken);

    bool isAnon() const;

    void loadBlocks();
    void blockUser(QString userId, std::function<void()> onSuccess,
                   std::function<void()> onFailure);
    void unblockUser(QString userId, std::function<void()> onSuccess,
                     std::function<void()> onFailure);

    void checkFollow(const QString targetUserID,
                     std::function<void(FollowResult)> onFinished);

    AccessGuard<const std::set<QString>> accessBlockedUserIds() const;
    AccessGuard<const std::set<TwitchUser>> accessBlocks() const;

    void loadEmotes();
    void loadUserstateEmotes(QStringList emoteSetKeys);
    SharedAccessGuard<const TwitchAccountEmoteData> accessEmotes() const;

    // Automod actions
    void autoModAllow(const QString msgID);
    void autoModDeny(const QString msgID);

private:
    void loadEmoteSetData(std::shared_ptr<EmoteSet> emoteSet);

    QString oauthClient_;
    QString oauthToken_;
    QString userName_;
    QString userId_;
    const bool isAnon_;
    Atomic<QColor> color_;

    mutable std::mutex ignoresMutex_;
    QElapsedTimer userstateEmotesTimer_;
    UniqueAccess<std::set<TwitchUser>> ignores_;
    UniqueAccess<std::set<QString>> ignoresUserIds_;

    //    std::map<UserId, TwitchAccountEmoteData> emotes;
    UniqueAccess<TwitchAccountEmoteData> emotes_;
};

}  // namespace chatterino

#pragma once

#include "common/network/NetworkRequest.hpp"

#include <QJsonArray>
#include <QJsonObject>

#include <functional>

namespace chatterino {

using IvrFailureCallback = std::function<void()>;
template <typename... T>
using ResultCallback = std::function<void(T...)>;

struct IvrSubage {
    const bool isSubHidden;
    const bool isSubbed;
    const QString subTier;
    const int totalSubMonths;
    const QString followingSince;
    const bool isGifted;
    const QString giftedBy;

    IvrSubage(const QJsonObject &root)
        : isSubHidden(root.value("statusHidden").toBool())
        , isSubbed(!root.value("meta").isNull())
        , subTier(root.value("meta").toObject().value("tier").toString())
        , totalSubMonths(
              root.value("cumulative").toObject().value("months").toInt())
        , followingSince(root.value("followedAt").toString())
        , isGifted(!root.value("meta").toObject().value("giftMeta").isNull())
        , giftedBy(root.value("meta").toObject().value("giftMeta").toObject().value("gifter").isNull() ? "anonymous" : root.value("meta").toObject().value("giftMeta").toObject().value("gifter").toObject().value("login").toString())
    {
    }
};

struct IvrResolve {
    const bool isPartner;
    const bool isAffiliate;
    const bool isBot;
    const bool isStaff;
    const bool isExStaff;
    const QString banReason;
    const QString userColor;
    const QString userBio;

    IvrResolve(QJsonArray arr)
        : isPartner(arr.at(0)
                        .toObject()
                        .value("roles")
                        .toObject()
                        .value("isPartner")
                        .toBool())
        , isAffiliate(arr.at(0)
                          .toObject()
                          .value("roles")
                          .toObject()
                          .value("isAffiliate")
                          .toBool())
        , isBot(arr.at(0).toObject().value("verifiedBot").toBool())
        , isStaff(arr.at(0)
                      .toObject()
                      .value("roles")
                      .toObject()
                      .value("isStaff")
                      .toBool())
        , isExStaff(!arr.at(0)
                         .toObject()
                         .value("roles")
                         .toObject()
                         .value("isStaff")
                         .isNull() &&
                    !arr.at(0)
                         .toObject()
                         .value("roles")
                         .toObject()
                         .value("isStaff")
                         .toBool() &&
                    !arr.at(0).isUndefined())
        , banReason(arr.at(0).toObject().value("banReason").toString().isEmpty()
                        ? "User does not exist"
                        : arr.at(0).toObject().value("banReason").toString())
        , userColor(arr.at(0).toObject().value("chatColor").toString().isEmpty()
                        ? "Default"
                        : arr.at(0).toObject().value("chatColor").toString())
        , userBio(arr.at(0).toObject().value("bio").toString().isEmpty()
                      ? "Empty"
                      : arr.at(0).toObject().value("bio").toString())
    {
    }
};

class IvrApi final
{
public:
    // https://api.ivr.fi/v2/docs/static/index.html#/Twitch/get_twitch_subage__user___channel_
    void getSubage(QString userName, QString channelName,
                   ResultCallback<IvrSubage> resultCallback,
                   IvrFailureCallback failureCallback);

    // https://api.ivr.fi/v2/docs/static/index.html#/Twitch/get_twitch_user
    void getUserData(QString userName,
                     ResultCallback<chatterino::IvrResolve> resultCallback,
                     IvrFailureCallback failureCallback);

    static void initialize();

    IvrApi() = default;

    IvrApi(const IvrApi &) = delete;
    IvrApi &operator=(const IvrApi &) = delete;

    IvrApi(IvrApi &&) = delete;
    IvrApi &operator=(IvrApi &&) = delete;

private:
    NetworkRequest makeRequest(QString url, QUrlQuery urlQuery);
};

IvrApi *getIvr();

}  // namespace chatterino

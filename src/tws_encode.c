#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <float.h>
#include <stdio.h>

#include <twsclient/tws_type.h>

#define __TWS_INTERNAL__
#include "tws_private.h"

/* outgoint msg ids */
#define REQ_MKT_DATA                    1
#define CANCEL_MKT_DATA                 2
#define PLACE_ORDER                     3
#define CANCEL_ORDER                    4
#define REQ_OPEN_ORDERS                 5
#define REQ_ACCOUNT_DATA                6
#define REQ_EXECUTIONS                  7
#define REQ_IDS                         8
#define REQ_CONTRACT_DATA               9
#define REQ_MKT_DEPTH                   10
#define CANCEL_MKT_DEPTH                11
#define REQ_NEWS_BULLETINS              12
#define CANCEL_NEWS_BULLETINS           13
#define SET_SERVER_LOGLEVEL             14
#define REQ_AUTO_OPEN_ORDERS            15
#define REQ_ALL_OPEN_ORDERS             16
#define REQ_MANAGED_ACCTS               17
#define REQ_FA                          18
#define REPLACE_FA                      19
#define REQ_HISTORICAL_DATA             20
#define EXERCISE_OPTIONS                21
#define REQ_SCANNER_SUBSCRIPTION        22
#define CANCEL_SCANNER_SUBSCRIPTION     23
#define REQ_SCANNER_PARAMETERS          24
#define CANCEL_HISTORICAL_DATA          25
#define REQ_CURRENT_TIME                49
#define REQ_REAL_TIME_BARS              50
#define CANCEL_REAL_TIME_BARS           51
#define REQ_FUNDAMENTAL_DATA            52
#define CANCEL_FUNDAMENTAL_DATA         53
#define REQ_CALC_IMPLIED_VOLAT          54
#define REQ_CALC_OPTION_PRICE           55
#define CANCEL_CALC_IMPLIED_VOLAT       56
#define CANCEL_CALC_OPTION_PRICE        57
#define REQ_GLOBAL_CANCEL               58
#define REQ_MARKET_DATA_TYPE            59
#define REQ_POSITIONS                   61
#define REQ_ACCOUNT_SUMMARY             62
#define CANCEL_ACCOUNT_SUMMARY          63
#define CANCEL_POSITIONS                64
#define VERIFY_REQUEST                  65
#define VERIFY_MESSAGE                  66
#define QUERY_DISPLAY_GROUPS            67
#define SUBSCRIBE_TO_GROUP_EVENTS       68
#define UPDATE_DISPLAY_GROUP            69
#define UNSUBSCRIBE_FROM_GROUP_EVENTS   70
#define START_API                       71

/* marshal format */
#define INT_FMT "%d"
#define DOUBLE_FMT "%.10g"
#define TIME_FMT "%Y%m%d %H:%M:%S %Z"

typedef enum {
    COMBO_FOR_REQUEST_MARKET_DATA,
    COMBO_FOR_REQUEST_HIST_DATA,
    COMBO_FOR_PLACE_ORDER,
} send_contract_combolegs_mode;

typedef enum {
    WITH_PRIMARY_EXCH,
    WITHOUT_PRIMARY_EXCH
}send_contract_mode;

/* marshal API */
static void send_int32(real_client_t *client, int32_t value)
{
    snprintf(client->wbuf + client->wlen, sizeof(client->wbuf) - client->wlen, INT_FMT, value);
    client->wlen += strlen(client->wbuf + client->wlen) + 1;
}

static inline void send_eol(real_client_t *client)
{
    char *p = client->wbuf + client->wlen;
    *p = '\0';
    client->wlen++;
}

static void send_int32_max(real_client_t *client, int32_t value)
{
    if (value == INT32_MAX)
        send_eol(client);
    else
        send_int32(client, value);
}

static inline void send_cmd(real_client_t *client, int cmd)
{
    send_int32(client, cmd);
}

static inline void send_bool(real_client_t *client, bool value)
{
    send_int32(client, value ? 1 : 0);
}

static void send_str(real_client_t *client, const char *str)
{
    if (IS_EMPTY(str)) {
        send_eol(client);
    } else {
        snprintf(client->wbuf + client->wlen, sizeof(client->wbuf) - client->wlen, "%s", str);
        client->wlen += strlen(client->wbuf + client->wlen) + 1;
    }
}

static void send_double(real_client_t *client, double value)
{
    snprintf(client->wbuf + client->wlen, sizeof(client->wbuf) - client->wlen, DOUBLE_FMT, value);
    client->wlen += strlen(client->wbuf + client->wlen) + 1;
}

static void send_double_max(real_client_t *client, double value)
{
    if (value == DBL_MAX)
        send_eol(client);
    else
        send_double(client, value);
}

static inline void flush_buf(real_client_t *client)
{
    tws_client_write(client, NULL);
}

/* Helper API */
static void send_contract_base(real_client_t *client, const tr_contract_t *contract, send_contract_mode mode)
{
    send_int32(client, contract->conId);
    send_str(client, contract->symbol);
    send_str(client, contract->secType);
    send_str(client, contract->expiry);
    send_double(client, contract->strike);
    send_str(client, contract->right);
    send_str(client, contract->multiplier);
    send_str(client, contract->exchange);
    if (WITH_PRIMARY_EXCH == mode)
        send_str(client, contract->primaryExch);
    send_str(client, contract->currency);
    send_str(client, contract->localSymbol);
    send_str(client, contract->tradingClass);
}

static void send_contract_combolegs(real_client_t *client, const tr_contract_t *contract,
                           const send_contract_combolegs_mode mode)
{
    int j;

    send_int32(client, contract->comboLegsCount);

    for (j = 0; j < contract->comboLegsCount; j++) {
        tr_comboleg_t *cl = &contract->comboLegs[j];

        send_int32(client, cl->conId);
        send_int32(client, cl->ratio);
        send_str(client, cl->action);
        send_str(client, cl->exchange);

        if (mode != COMBO_FOR_REQUEST_MARKET_DATA && mode != COMBO_FOR_REQUEST_HIST_DATA) {
            send_int32(client, cl->openClose);
            send_int32(client, cl->shortSaleSlot);
            send_str(client, cl->designatedLocation);
            send_int32(client, cl->exemptCode);
        }
    }
}

static void send_under_comp(real_client_t *client, const tr_contract_t *contract)
{
    if (contract->underComp) {
        send_int32(client, 1);
        send_int32(client, contract->underComp->conId);
        send_double(client, contract->underComp->delta);
        send_double(client, contract->underComp->price);
    } else {
        send_int32(client, 0);
    }
}

/*
 * @Return 0 on error, !0 on successfully sending the tag list
 */
static int send_tag_list(real_client_t *client, const tr_tag_value_t *list, int list_size)
{
    send_int32(client, list_size);

    if (list_size > 0) {
        int j;

        if (list == NULL) {
            return 0;
        } else {
            for (j = 0; j < list_size; j++) {
                if (list[j].tag == NULL) {
                    return 0;
                }

                send_str(client, list[j].tag);
                send_str(client, list[j].val);
            }
        }
    }

    return 1;
}

/* @Return: 1 success
 *          0 memory failed or taglist error
 */
static int send_options_list(real_client_t *client, const tr_tag_value_t *list, int list_size)
{
    if (NULL == list || list_size <= 0) {
        send_eol(client);
        return 1;
    }

    char sbuf[1024];
    char obuf[4096];
    unsigned int index = 0;

    for (int j = 0; j < list_size; j++) {
        if (list[j].tag == NULL) {
            return 0;
        }

        snprintf(sbuf, sizeof(sbuf), "%s=%s;", list[j].tag, list[j].val);
        int len = strlen(sbuf);

        if (index + len >= sizeof(obuf) || sbuf[len - 1] != ';') {
            return 0;
        }

        memmove(&obuf[index], sbuf, strlen(sbuf));
        index += len;
    }

    obuf[index] = '\0';
    send_str(client, obuf);
    return 1;
}

/* Internal API */
static void on_start_api(real_client_t *client)
{
    client->state = TWS_ST_READY;
    if (client->event_cb != NULL)
        client->event_cb((tws_client_t *)client, CLIENT_CONNECTED, NULL);
}

void tws_client_startAPI(real_client_t *client)
{
    if (client->state > TWS_ST_AUTHED) return;

    int VERSION = 1;

    send_cmd(client, START_API);
    send_int32(client, VERSION);
    send_int32(client, client->clientId);
    tws_client_write(client, on_start_api);
}

void tws_client_logon(real_client_t *client)
{
    /* start to logon tws */
    send_int32(client, CLIENT_VERSION);
    tws_client_write(client, NULL);
}

#define CHECK_CONNECTED real_client_t *client = (real_client_t *)c;\
    do { if (client->state < TWS_ST_READY) return; }while(0)

/* Public Command API */
void tws_client_cancel_scanner_subscription(tws_client_t *c, int tickerId)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, CANCEL_SCANNER_SUBSCRIPTION);
    send_int32(client, VERSION);
    send_int32(client, tickerId);
    flush_buf(client);
}

void tws_client_req_scanner_parameters(tws_client_t *c)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, REQ_SCANNER_PARAMETERS);
    send_int32(client, VERSION);
    flush_buf(client);
}

void tws_client_req_scanner_subscription(tws_client_t *c, int tickerId,
                                         tr_scanner_subscription_t *subscription,
                                         tr_tag_value_t *options, int optionsCount)
{
    CHECK_CONNECTED;

    int VERSION = 4;
    send_cmd(client, REQ_SCANNER_SUBSCRIPTION);
    send_int32(client, VERSION);
    send_int32(client, tickerId);
    send_int32_max(client, subscription->numberOfRows);
    send_str(client, subscription->instrument);
    send_str(client, subscription->locationCode);
    send_str(client, subscription->scanCode);
    send_double_max(client, subscription->abovePrice);
    send_double_max(client, subscription->belowPrice);
    send_double_max(client, subscription->aboveVolume);
    send_double_max(client, subscription->marketCapAbove);
    send_double_max(client, subscription->marketCapBelow);
    send_str(client, subscription->moodyRatingAbove);
    send_str(client, subscription->moodyRatingBelow);
    send_str(client, subscription->spRatingAbove);
    send_str(client, subscription->spRatingBelow);
    send_str(client, subscription->maturityDateAbove);
    send_str(client, subscription->maturityDateBelow);
    send_double_max(client, subscription->couponRateAbove);
    send_double_max(client, subscription->couponRateBelow);
    send_str(client, subscription->excludeConvertible);
    send_double_max(client, subscription->averageOptionVolumeAbove);
    send_str(client, subscription->scannerSettingPairs);
    send_str(client, subscription->stockTypeFilter);
    send_options_list(client, options, optionsCount);
    flush_buf(client);
}

void tws_client_req_mkt_data(tws_client_t *c, int tickerId,
                             tr_contract_t *contract, const char *generick_tick_list,
                             bool snapshot, tr_tag_value_t *options, int optionsCount)
{
    CHECK_CONNECTED;

    int VERSION = 11;
    send_cmd(client, REQ_MKT_DATA);
    send_int32(client, VERSION);
    send_int32(client, tickerId);

    send_contract_base(client, contract, WITH_PRIMARY_EXCH);

    if ((contract->secType != 0) && (ascii_strcasecmp(contract->secType, "BAG") == 0))
        send_contract_combolegs(client, contract, COMBO_FOR_REQUEST_MARKET_DATA);

    send_under_comp(client, contract);

    send_str(client, generick_tick_list);

    send_bool(client, snapshot);

    send_options_list(client, options, optionsCount);

    flush_buf(client);
}

void tws_client_cancel_historical_data(tws_client_t *c, int tickerId)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, CANCEL_HISTORICAL_DATA);
    send_int32(client, VERSION);
    send_int32(client, tickerId);
    flush_buf(client);
}

void tws_client_cancel_realtime_bars(tws_client_t *c, int tickerId)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, CANCEL_REAL_TIME_BARS);
    send_int32(client, VERSION);
    send_int32(client, tickerId);
    flush_buf(client);
}

void tws_client_req_historical_data(tws_client_t *c, int tickerId,
                                    tr_contract_t *contract, const char *endDataTime,
                                    const char *durationStr, const char *barSizeSetting,
                                    const char *whatToShow, bool useRTH, int formatDate,
                                    tr_tag_value_t *options, int optionsCount)
{
    CHECK_CONNECTED;

    int VERSION = 6;
    send_cmd(client, REQ_HISTORICAL_DATA);
    send_int32(client, VERSION);
    send_int32(client, tickerId);
    send_contract_base(client, contract, WITH_PRIMARY_EXCH);
    send_int32(client, contract->includeExpired ? 1 : 0);
    send_str(client, endDataTime);
    send_str(client, barSizeSetting);
    send_str(client, durationStr);
    send_bool(client, useRTH);
    send_str(client, whatToShow);
    send_int32(client, formatDate);

    if ((contract->secType != 0) && (ascii_strcasecmp(contract->secType, "BAG") == 0))
        send_contract_combolegs(client, contract, COMBO_FOR_REQUEST_HIST_DATA);

    send_options_list(client, options, optionsCount);


    flush_buf(client);
}

void tws_client_req_realtime_bars(tws_client_t *c, int tickerId,
                                  tr_contract_t *contract, int barSize,
                                  const char *whatToShow, bool useRTH,
                                  tr_tag_value_t *options, int optionsCount)
{
    CHECK_CONNECTED;

    int VERSION = 3;
    send_cmd(client, REQ_REAL_TIME_BARS);
    send_int32(client, VERSION);
    send_int32(client, tickerId);

    send_contract_base(client, contract, WITH_PRIMARY_EXCH);

    send_int32(client, barSize);
    send_str(client, whatToShow);
    send_bool(client, useRTH);

    send_options_list(client, options, optionsCount);

    flush_buf(client);
}

void tws_client_req_contract_details(tws_client_t *c, int reqId,
                                     tr_contract_t *contract)
{
    CHECK_CONNECTED;

    int VERSION = 7;
    send_cmd(client, REQ_CONTRACT_DATA);
    send_int32(client, VERSION);
    send_int32(client, reqId);

    send_contract_base(client, contract, WITHOUT_PRIMARY_EXCH);

    send_int32(client, contract->includeExpired);
    send_str(client, contract->secIdType);
    send_str(client, contract->secId);


    flush_buf(client);
}

void tws_client_req_mkt_depth(tws_client_t *c, int tickerId,
                             tr_contract_t *contract, int numRows,
                             tr_tag_value_t *options, int optionsCount)
{
    CHECK_CONNECTED;

    int VERSION = 5;
    send_cmd(client, REQ_MKT_DEPTH);
    send_int32(client, VERSION);
    send_int32(client, tickerId);

    send_contract_base(client, contract, WITHOUT_PRIMARY_EXCH);

    send_int32(client, numRows);

    send_options_list(client, options, optionsCount);

    flush_buf(client);
}

void tws_client_cancel_mkt_data(tws_client_t *c, int tickerId)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, CANCEL_MKT_DATA);
    send_int32(client, VERSION);
    send_int32(client, tickerId);

    flush_buf(client);
}

void tws_client_cancel_mkt_depth(tws_client_t *c, int tickerId)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, CANCEL_MKT_DEPTH);
    send_int32(client, VERSION);
    send_int32(client, tickerId);

    flush_buf(client);
}

void tws_client_exercise_options(tws_client_t *c, int tickerId,
                                 tr_contract_t *contract, int exerciseAction,
                                 int exerciseQuantity, const char *account, int override)
{
    CHECK_CONNECTED;

    int VERSION = 2;
    send_cmd(client, EXERCISE_OPTIONS);
    send_int32(client, VERSION);
    send_int32(client, tickerId);

    send_contract_base(client, contract, WITHOUT_PRIMARY_EXCH);


    send_int32(client, exerciseAction);
    send_int32(client, exerciseQuantity);
    send_str(client, account);
    send_int32(client, override);

    flush_buf(client);
}

void tws_client_place_order(tws_client_t *c, int id,
                                 tr_contract_t *contract, tr_order_t *order)
{
    CHECK_CONNECTED;

    int VERSION = 42;
    send_cmd(client, PLACE_ORDER);
    send_int32(client, VERSION);
    send_int32(client, id);

    send_contract_base(client, contract, WITH_PRIMARY_EXCH);
    send_str(client, contract->secIdType);
    send_str(client, contract->secId);

    /* send main order fields */
    send_str(client, order->action);
    send_int32(client, order->totalQuantity);
    send_str(client, order->orderType);
    send_double_max(client, order->lmtPrice);
    send_double_max(client, order->auxPrice);
    /* send extended order fields */
    send_str(client, order->tif);
    send_str(client, order->ocaGroup);
    send_str(client, order->account);
    send_str(client, order->openClose);
    send_int32(client, order->origin);
    send_str(client, order->orderRef);
    send_bool(client, order->transmit);
    send_int32(client, order->parentId);
    send_bool(client, order->blockOrder);
    send_bool(client, order->sweepToFill);
    send_int32(client, order->displaySize);
    send_int32(client, order->triggerMethod);
    send_bool(client, order->outsideRth);
    send_bool(client, order->hidden);

    if ((contract->secType != 0) && (ascii_strcasecmp(contract->secType, "BAG") == 0)) {
        send_contract_combolegs(client, contract, COMBO_FOR_PLACE_ORDER);
        send_int32(client, order->comboLegsCount);
        for (int j = 0; j < order->comboLegsCount; j++) {
            tr_order_combo_leg_t *leg = &order->comboLegs[j];
            send_double_max(client, leg->price);
        }
        send_tag_list(client, order->smartComboRoutingParams, order->smartComboRoutingParamsCount);
    }

    send_str(client, ""); // send deprecated sharesAllocation field
    send_double(client, order->discretionaryAmt);
    send_str(client, order->goodAfterTime);
    send_str(client, order->goodTillDate);
    send_str(client, order->faGroup);
    send_str(client, order->faMethod);
    send_str(client, order->faPercentage);
    send_str(client, order->faProfile);

    send_int32(client, order->shortSaleSlot);
    send_str(client, order->designatedLocation);
    send_int32(client, order->exemptCode);
    send_int32(client, order->ocaType);
    send_str(client, order->rule80A);
    send_str(client, order->settlingFirm);
    send_bool(client, order->allOrNone);
    send_int32_max(client, order->minQty);
    send_double_max(client, order->percentOffset);
    send_bool(client, order->eTradeOnly);
    send_bool(client, order->firmQuoteOnly);
    send_double_max(client, order->nbboPriceCap);
    send_int32_max(client, order->auctionStrategy);
    send_double_max(client, order->startingPrice);
    send_double_max(client, order->stockRefPrice);
    send_double_max(client, order->delta);
    send_double_max(client, order->stockRangeLower);
    send_double_max(client, order->stockRangeUpper);
    send_bool(client, order->overridePercentageConstraints);
    send_double_max(client, order->volatility);
    send_int32_max(client, order->volatilityType);
    send_str(client, order->deltaNeutralOrderType);
    send_double_max(client, order->deltaNeutralAuxPrice);
    if (!IS_EMPTY(order->deltaNeutralOrderType)) {
        send_int32(client, order->deltaNeutralConId);
        send_str(client, order->deltaNeutralSettlingFirm);
        send_str(client, order->deltaNeutralClearingAccount);
        send_str(client, order->deltaNeutralClearingIntent);
        send_str(client, order->deltaNeutralOpenClose);
        send_int32(client, order->deltaNeutralShortSale);
        send_int32(client, order->deltaNeutralShortSaleSlot);
        send_str(client, order->deltaNeutralDesignatedLocation);
    }
    send_bool(client, order->continuousUpdate);
    send_int32_max(client, order->referencePriceType);
    send_double_max(client, order->trailStopPrice);
    send_double_max(client, order->trailingPercent);
    send_int32_max(client, order->scaleInitLevelSize);
    send_int32_max(client, order->scaleSubsLevelSize);
    send_double_max(client, order->scalePriceIncrement);

    if (order->scalePriceIncrement > 0.0 && order->scalePriceIncrement != DBL_MAX) {
        send_double_max(client, order->scalePriceAdjustValue);
        send_int32_max(client, order->scalePriceAdjustInterval);
        send_double_max(client, order->scaleProfitOffset);
        send_bool(client, order->scaleAutoReset);
        send_int32_max(client, order->scaleInitPosition);
        send_int32_max(client, order->scaleInitFillQty);
        send_bool(client, order->scaleRandomPercent);
    }

    send_str(client, order->scaleTable);
    send_str(client, order->activeStartTime);
    send_str(client, order->activeStopTime);

    /* HEDGE orders */
    send_str(client, order->hedgeType);
    if (!IS_EMPTY(order->hedgeType))
        send_str(client, order->hedgeParam);

    send_bool(client, order->optOutSmartRouting);

    send_str(client, order->clearingAccount);
    send_str(client, order->clearingIntent);

    send_bool(client, order->notHeld);

    send_under_comp(client, contract);

    send_str(client, order->algoStrategy);
    if (!IS_EMPTY(order->algoStrategy)) {
        send_tag_list(client, order->algoParams, order->algoParamsCount);
    }

    send_bool(client, order->whatIf);

    if (order->orderMiscOptions)
                send_options_list(client, order->orderMiscOptions, order->orderMiscOptionsCount);

    flush_buf(client);
}

void tws_client_req_account_updates(tws_client_t *c, bool subscribe, const char *acctCode)
{
    CHECK_CONNECTED;

    int VERSION = 2;
    send_cmd(client, REQ_ACCOUNT_DATA);
    send_int32(client, VERSION);
    send_bool(client, subscribe);
    send_str(client, acctCode);

    flush_buf(client);
}

void tws_client_req_executions(tws_client_t *c, int reqId, tr_exec_filter_t *filter)
{
    CHECK_CONNECTED;

    int VERSION = 3;
    send_cmd(client, REQ_EXECUTIONS);
    send_int32(client, VERSION);
    send_int32(client, reqId);

    send_int32(client, filter->clientId);
    send_str(client, filter->acctCode);

            /* Note that the valid format for m_time is "yyyymmdd-hh:mm:ss" */
    send_str(client, filter->time);
    send_str(client, filter->symbol);
    send_str(client, filter->secType);
    send_str(client, filter->exchange);
    send_str(client, filter->side);

    flush_buf(client);
}

void tws_client_cancel_order(tws_client_t *c, int id)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, CANCEL_ORDER);
    send_int32(client, VERSION);
    send_int32(client, id);

    flush_buf(client);
}

void tws_client_req_open_orders(tws_client_t *c)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, REQ_OPEN_ORDERS);
    send_int32(client, VERSION);

    flush_buf(client);
}

void tws_client_req_ids(tws_client_t *c, int numIds)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, REQ_IDS);
    send_int32(client, VERSION);
    send_int32(client, numIds);

    flush_buf(client);
}

void tws_client_req_news_bulletins(tws_client_t *c, bool allMsgs)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, REQ_NEWS_BULLETINS);
    send_int32(client, VERSION);
    send_bool(client, allMsgs);

    flush_buf(client);
}

void tws_client_cancel_news_bulletins(tws_client_t *c)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, CANCEL_NEWS_BULLETINS);
    send_int32(client, VERSION);

    flush_buf(client);
}

void tws_client_set_server_loglevel(tws_client_t *c, int logLevel)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, SET_SERVER_LOGLEVEL);
    send_int32(client, VERSION);
    send_int32(client, logLevel);

    flush_buf(client);
}

void tws_client_req_auto_open_orders(tws_client_t *c, bool bAutoBind)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, REQ_AUTO_OPEN_ORDERS);
    send_int32(client, VERSION);
    send_bool(client, bAutoBind);

    flush_buf(client);
}

void tws_client_req_all_open_orders(tws_client_t *c)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, REQ_ALL_OPEN_ORDERS);
    send_int32(client, VERSION);

    flush_buf(client);
}

void tws_client_req_managed_accts(tws_client_t *c)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, REQ_MANAGED_ACCTS);
    send_int32(client, VERSION);

    flush_buf(client);
}

void tws_client_req_fa(tws_client_t *c, int faDataType)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, REQ_FA);
    send_int32(client, VERSION);
    send_int32(client, faDataType);

    flush_buf(client);
}

void tws_client_replace_fa(tws_client_t *c, int faDataType, const char *xml)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, REPLACE_FA);
    send_int32(client, VERSION);
    send_int32(client, faDataType);
    send_str(client, xml);

    flush_buf(client);
}

void tws_client_req_current_time(tws_client_t *c)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, REQ_CURRENT_TIME);
    send_int32(client, VERSION);

    flush_buf(client);
}

void tws_client_req_fundamenta_data(tws_client_t *c, int reqId,
                                    tr_contract_t *contract,
                                    const char *reportType)
{
    CHECK_CONNECTED;

    int VERSION = 2;
    send_cmd(client, REQ_FUNDAMENTAL_DATA);
    send_int32(client, VERSION);
    send_int32(client, reqId);

    send_int32(client, contract->conId);
    send_str(client, contract->symbol);
    send_str(client, contract->secType);
    send_str(client, contract->exchange);
    send_str(client, contract->primaryExch);
    send_str(client, contract->currency);
    send_str(client, contract->localSymbol);

    send_str(client, reportType);


    flush_buf(client);
}

void tws_client_cancel_fundamenta_data(tws_client_t *c, int reqId)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, CANCEL_FUNDAMENTAL_DATA);
    send_int32(client, VERSION);
    send_int32(client, reqId);

    flush_buf(client);
}

void tws_client_calculate_implied_volatility(tws_client_t *c, int reqId,
                                             tr_contract_t *contract,
                                             double optionPrice, double underPrice)
{
    CHECK_CONNECTED;

    int VERSION = 2;
    send_cmd(client, REQ_CALC_IMPLIED_VOLAT);
    send_int32(client, VERSION);
    send_int32(client, reqId);

    send_contract_base(client, contract, WITH_PRIMARY_EXCH);

    send_double(client, optionPrice);
    send_double(client, underPrice);

    flush_buf(client);
}

void tws_client_cancel_calculate_implied_volatility(tws_client_t *c, int reqId)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, CANCEL_CALC_IMPLIED_VOLAT);
    send_int32(client, VERSION);
    send_int32(client, reqId);

    flush_buf(client);
}

void tws_client_calculate_option_price(tws_client_t *c, int reqId,
                                             tr_contract_t *contract,
                                             double volatility, double underPrice)
{
    CHECK_CONNECTED;

    int VERSION = 2;
    send_cmd(client, REQ_CALC_OPTION_PRICE);
    send_int32(client, VERSION);
    send_int32(client, reqId);

    send_contract_base(client, contract, WITH_PRIMARY_EXCH);

    send_double(client, volatility);
    send_double(client, underPrice);

    flush_buf(client);
}

void tws_client_cancel_calculate_option_price(tws_client_t *c, int reqId)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, CANCEL_CALC_OPTION_PRICE);
    send_int32(client, VERSION);
    send_int32(client, reqId);

    flush_buf(client);
}

void tws_client_req_global_cancel(tws_client_t *c)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, REQ_GLOBAL_CANCEL);
    send_int32(client, VERSION);

    flush_buf(client);
}

void tws_client_req_market_data_type(tws_client_t *c, int marketDataType)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, REQ_MARKET_DATA_TYPE);
    send_int32(client, VERSION);
    send_int32(client, marketDataType);

    flush_buf(client);
}

void tws_client_req_positions(tws_client_t *c)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, REQ_POSITIONS);
    send_int32(client, VERSION);

    flush_buf(client);
}

void tws_client_cancel_positions(tws_client_t *c)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, CANCEL_POSITIONS);
    send_int32(client, VERSION);

    flush_buf(client);
}

void tws_client_req_account_summary(tws_client_t *c, int reqId, const char *group, const char *tags)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, REQ_ACCOUNT_SUMMARY);
    send_int32(client, VERSION);
    send_int32(client, reqId);
    send_str(client, group);
    send_str(client, tags);

    flush_buf(client);
}

void tws_client_cancel_account_summary(tws_client_t *c, int reqId)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, CANCEL_ACCOUNT_SUMMARY);
    send_int32(client, VERSION);
    send_int32(client, reqId);

    flush_buf(client);
}

void tws_client_verify_request(tws_client_t *c, const char *apiName, const char *apiVersion)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, VERIFY_REQUEST);
    send_int32(client, VERSION);
    send_str(client, apiName);
    send_str(client, apiVersion);

    flush_buf(client);
}

void tws_client_verify_message(tws_client_t *c, const char *apiData)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, VERIFY_MESSAGE);
    send_int32(client, VERSION);
    send_str(client, apiData);

    flush_buf(client);
}

void tws_client_query_display_groups(tws_client_t *c, int reqId)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, QUERY_DISPLAY_GROUPS);
    send_int32(client, VERSION);
    send_int32(client, reqId);

    flush_buf(client);
}

void tws_client_subscribe_to_group_events(tws_client_t *c, int reqId, int groupId)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, SUBSCRIBE_TO_GROUP_EVENTS);
    send_int32(client, VERSION);
    send_int32(client, reqId);
    send_int32(client, groupId);

    flush_buf(client);
}

void tws_client_update_display_group(tws_client_t *c, int reqId, const char *contractInfo)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, UPDATE_DISPLAY_GROUP);
    send_int32(client, VERSION);
    send_int32(client, reqId);
    send_str(client, contractInfo);

    flush_buf(client);
}

void tws_client_unsubscribe_from_group_events(tws_client_t *c, int reqId)
{
    CHECK_CONNECTED;

    int VERSION = 1;
    send_cmd(client, UNSUBSCRIBE_FROM_GROUP_EVENTS);
    send_int32(client, VERSION);
    send_int32(client, reqId);

    flush_buf(client);
}

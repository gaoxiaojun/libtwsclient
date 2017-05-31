#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include <twsclient/tws_type.h>
#include <twsclient/tws_event.h>
#include <twsclient/tws_error.h>

#include "pt.h"

#define __TWS_INTERNAL__
#include "tws_private.h"

/* marshal API */
static inline int read_int32_max(char *p)
{
    return IS_EMPTY(p) ? INT32_MAX : atoi(p);
}

static inline double read_double_max(char *p)
{
    return IS_EMPTY(p) ? DBL_MAX : atof(p);
}

static inline int read_bool(char *p)
{
    return !!atoi(p);
}

static inline char *read_str(char *p)
{
    if (!IS_EMPTY(p))
        return sk_strdup(p);
    else
        return NULL;
}

#define PBEGIN      PT_BEGIN(&client->pt)
#define PEND        PT_END(&client->pt)
#define YIELD       PT_YIELD(&client->pt)

/* if NOT use YIELD in function,
 * don't using PBEGIN & PEND,
 * replace with NBDGIN & NEND
 */
#define NBEGIN
#define NEND        return PT_ENDED

#define CALLBACK(type) \
    do { \
        if (client->event_cb != NULL) \
            client->event_cb((tws_client_t *)client, type, &client->event); \
    }while(0)

#define RINT        atoi(token)
#define RLONG       atol(token)
#define RDOUBLE     atof(token)
#define RDOUBLEMAX  read_double_max(token)
#define RINTMAX     read_int32_max(token)
#define RSTRING     read_str(token)
#define RBOOL       read_bool(token)


/* Decoder API */
static char decode_login_package(real_client_t *client, char *token)
{
    PBEGIN;
        client->serverVersion = RINT;
        if (client->serverVersion >= 20) {
            YIELD; /* ignore server time */
        }

        if (client->serverVersion < MIN_REQ_SERVER_VERSION) {
            client->event.data.err_msg.errorCode = UPDATE_TWS;
            CALLBACK(ERR_MSG);
            tws_client_close((tws_client_t *)client);
        } else {
            client->state = TWS_ST_AUTHED;
            memset(client->rbuf, 0, sizeof(client->rbuf));
            tws_decode_reset(client);

            tws_client_startAPI(client);
        }
    PEND;
}

#define ud client->event.data.tick_price

static char receive_tick_price(real_client_t *client, char *token)
{
    PBEGIN;
        client->event.version = RINT;
        YIELD; ud.tickerId = RINT;
        YIELD; ud.tickType = RINT;
        YIELD; ud.price = RDOUBLE;
        ud.size = 0;
        if (client->event.version >= 2) {
            YIELD; ud.size = RINT;
        }
        ud.canAutoExecute = 0;
        if (client->event.version >=3) {
            YIELD; ud.canAutoExecute = RINT;
        }
        if (client->event.version >= 2) {
            ud.sizeTickType = -1;
            if (ud.tickType == 1) ud.sizeTickType = 0;    // BID_SIZE
            else if (ud.tickType == 2) ud.sizeTickType = 3;    // ASK_SIZE
            else if (ud.tickType == 4) ud.sizeTickType = 5;    // LAST_SIZE
        }

        CALLBACK(TICK_PRICE);
    PEND;
}
#undef ud

#define ud client->event.data.tick_size
static char receive_tick_size(real_client_t *client, char *token)
{
    PBEGIN;
        /* ignore version */
        YIELD; ud.tickerId = RINT;
        YIELD; ud.tickType = RINT;
        YIELD; ud.size = RINT;

        CALLBACK(TICK_SIZE);
    PEND;
}
#undef ud

#define ud client->event.data.position
static char receive_position(real_client_t *client, char *token)
{
    PBEGIN;
        ud.contract = sk_malloc(sizeof(tr_contract_t));
        tws_init_contract(ud.contract);

        client->event.version = RINT;
        YIELD; ud.account = RSTRING;
        YIELD; ud.contract->conId = RINT;
        YIELD; ud.contract->symbol = RSTRING;
        YIELD; ud.contract->secType = RSTRING;
        YIELD; ud.contract->expiry = RSTRING;
        YIELD; ud.contract->strike = RDOUBLE;
        YIELD; ud.contract->right = RSTRING;
        YIELD; ud.contract->multiplier = RSTRING;
        YIELD; ud.contract->exchange = RSTRING;
        YIELD; ud.contract->currency = RSTRING;
        YIELD; ud.contract->localSymbol = RSTRING;
        if (client->event.version >= 2) {
            YIELD; ud.contract->tradingClass = RSTRING;
        }
        YIELD; ud.position = RINT;

        if (client->event.version >= 3) {
            YIELD; ud.avgCost = RDOUBLE;
        } else {
            ud.avgCost = 0;
        }

        CALLBACK(POSITION);

        /* free */
        tws_destroy_contract(ud.contract);
        sk_free(ud.contract);
    PEND;
}
#undef ud

#define ud client->event.data.position_end
static char receive_position_end(real_client_t *client, char *token)
{
    UNUSED(token);
    NBEGIN
        /* ignore version */
        CALLBACK(POSITION_END);
    NEND;
}
#undef ud

#define ud client->event.data.account_summary
static char receive_account_summary(real_client_t *client, char *token)
{
    PBEGIN;
        /* ignore version */
        YIELD; ud.reqId = RINT;
        YIELD; ud.account = RSTRING;
        YIELD; ud.tag = RSTRING;
        YIELD; ud.value = RSTRING;
        YIELD; ud.currency = RSTRING;

        CALLBACK(ACCOUNT_SUMMARY);
        /* free */
        sk_free(ud.account);
        sk_free(ud.tag);
        sk_free(ud.value);
        sk_free(ud.currency);
    PEND;
}
#undef ud

#define ud client->event.data.account_summary_end
static char receive_account_summary_end(real_client_t *client, char *token)
{
    PBEGIN;
        /* ignore version */
        YIELD;  ud.reqId = RINT;

        CALLBACK(ACCOUNT_SUMMARY_END);
    PEND;
}
#undef ud

#define ud client->event.data.tick_option_computation
static char receive_tick_option_computation(real_client_t *client, char *token)
{
    PBEGIN;
        ud.optPrice = DBL_MAX;
        ud.pvDividend = DBL_MAX;
        ud.gamma = DBL_MAX;
        ud.vega = DBL_MAX;
        ud.theta = DBL_MAX;
        ud.undPrice = DBL_MAX;

        client->event.version = RINT;

        YIELD; ud.tickerId = RINT;
        YIELD; ud.tickType = RINT;

        YIELD; ud.impliedVol = RDOUBLE;
        if (ud.impliedVol < 0) ud.impliedVol = DBL_MAX;

        YIELD; ud.delta = RDOUBLE;
        if (fabs(ud.delta) > 1) ud.delta = DBL_MAX;

        if (client->event.version >= 6 || ud.tickType == MODEL_OPTION) {
            YIELD; ud.optPrice = RDOUBLE;
            if (ud.optPrice < 0) ud.optPrice = DBL_MAX;

            YIELD; ud.pvDividend = RDOUBLE;
            if (ud.pvDividend < 0) ud.optPrice = DBL_MAX;
        }

        if (client->event.version >= 6) {
            YIELD; ud.gamma = RDOUBLE;
            if (fabs(ud.gamma) > 1) ud.gamma = DBL_MAX;

            YIELD; ud.vega = RDOUBLE;
            if (fabs(ud.vega) > 1) ud.vega = DBL_MAX;

            YIELD; ud.theta = RDOUBLE;
            if (fabs(ud.theta) > 1) ud.theta = DBL_MAX;

            YIELD; ud.undPrice = RDOUBLE;
            if (ud.undPrice < 0) ud.undPrice = DBL_MAX;
        }

        CALLBACK(TICK_OPTION_COMPUTATION);
    PEND;
}
#undef ud

#define ud client->event.data.tick_generic
static char receive_tick_generic(real_client_t *client, char *token)
{
    PBEGIN;
        /* ignore version */
        YIELD; ud.tickerId = RINT;
        YIELD; ud.tickType = RINT;
        YIELD; ud.value = RDOUBLE;

        CALLBACK(TICK_GENERIC);
    PEND;
}
#undef ud

#define ud client->event.data.tick_string
static char receive_tick_string(real_client_t *client, char *token)
{
    PBEGIN;
        /* ignore version */
        YIELD; ud.tickerId = RINT;
        YIELD; ud.tickType = RINT;
        YIELD; ud.value = RSTRING;

        CALLBACK(TICK_STRING);

        sk_free(ud.value);
    PEND;
}
#undef ud

#define ud client->event.data.tick_efp
static char receive_tick_efp(real_client_t *client, char *token)
{
    PBEGIN;
        /* ignore version */
        YIELD; ud.tickerId = RINT;
        YIELD; ud.tickType = RINT;
        YIELD; ud.basisPoints = RDOUBLE;
        YIELD; ud.formatteBasisPoints = RSTRING;
        YIELD; ud.impliedFuturesPrice = RDOUBLE;
        YIELD; ud.holdDays = RINT;
        YIELD; ud.futureExpiry = RSTRING;
        YIELD; ud.dividenImpact = RDOUBLE;
        YIELD; ud.dividensToExpiry = RDOUBLE;

        CALLBACK(TICK_EFP);

        sk_free(ud.formatteBasisPoints);
        sk_free(ud.futureExpiry);
    PEND;
}
#undef ud

#define ud client->event.data.order_status
static char receive_order_status(real_client_t *client, char *token)
{
    PBEGIN;
        client->event.version = RINT;
        YIELD; ud.id = RINT;
        YIELD; ud.status = RSTRING;
        YIELD; ud.filled = RINT;
        YIELD; ud.remaining = RINT;
        YIELD; ud.avgFillPrice = RDOUBLE;

        if (client->event.version >= 2) {
            YIELD; ud.permId = RINT;
        }

        if (client->event.version >= 3) {
            YIELD; ud.parentId = RINT;
        }

        if (client->event.version >= 4) {
            YIELD; ud.lastFillPrice = RDOUBLE;
        }

        if (client->event.version >= 5) {
            YIELD; ud.clientId = RINT;
        }

        ud.whyHeld = NULL;
        if (client->event.version >= 6) {
            YIELD; ud.whyHeld = RSTRING;
        }

        CALLBACK(ORDER_STATUS);

        /* free */
        sk_free(ud.status);
        sk_free(ud.whyHeld);
    PEND;
}
#undef ud

#define ud client->event.data.acct_value
static char receive_acct_value(real_client_t *client, char *token)
{
    PBEGIN;
        client->event.version = RINT;
        YIELD; ud.key = RSTRING;
        YIELD; ud.val = RSTRING;
        YIELD; ud.currency = RSTRING;

        ud.accoutName = NULL;
        if (client->event.version >= 2) {
            YIELD; ud.accoutName = RSTRING;
        }

        CALLBACK(ACCT_VALUE);
        sk_free(ud.key);
        sk_free(ud.val);
        sk_free(ud.currency);
        sk_free(ud.accoutName);
    PEND;
}
#undef ud

#define ud client->event.data.portfolio_value
static char receive_portfolio_value(real_client_t *client, char *token)
{
    PBEGIN;
        ud.contract = sk_malloc(sizeof(tr_contract_t));
        tws_init_contract(ud.contract);

        client->event.version = RINT;
        if (client->event.version >= 6) {
            YIELD; ud.contract->conId = RINT;
        }
        YIELD; ud.contract->symbol = RSTRING;
        YIELD; ud.contract->secType = RSTRING;
        YIELD; ud.contract->expiry = RSTRING;
        YIELD; ud.contract->strike = RDOUBLE;
        YIELD; ud.contract->right = RSTRING;
        if (client->event.version >= 7) {
            YIELD; ud.contract->multiplier = RSTRING;
            YIELD; ud.contract->primaryExch = RSTRING;
        }
        YIELD; ud.contract->currency = RSTRING;
        if (client->event.version >= 2) {
            YIELD; ud.contract->localSymbol = RSTRING;
        }
        if (client->event.version >= 8) {
            YIELD; ud.contract->tradingClass = RSTRING;
        }
        YIELD; ud.position = RINT;
        YIELD; ud.marketPrice = RDOUBLE;
        YIELD; ud.marketValue = RDOUBLE;
        ud.averageCost = 0.0;
        ud.unrealizedPNL = 0.0;
        ud.realizedPNL = 0.0;
        if (client->event.version >= 3) {
            YIELD; ud.averageCost = RDOUBLE;
            YIELD; ud.unrealizedPNL = RDOUBLE;
            YIELD; ud.realizedPNL = RDOUBLE;
        }

        ud.accountName = NULL;
        if (client->event.version >= 4) {
            YIELD; ud.accountName = RSTRING;
        }

        CALLBACK(PORTFOLIO_VALUE);

        tws_destroy_contract(ud.contract);
        sk_free(ud.contract);

        sk_free(ud.accountName);
    PEND;
}
#undef ud

#define ud client->event.data.acct_update_time
static char receive_acct_update_time(real_client_t *client, char *token)
{
    PBEGIN;
        /* ignore version */
        YIELD; ud.timeStamp = RSTRING;

        CALLBACK(ACCT_UPDATE_TIME);

        sk_free(ud.timeStamp);
    PEND;
}
#undef ud

#define ud client->event.data.err_msg
static char receive_err_msg(real_client_t *client, char *token)
{
    PBEGIN;
        client->event.version = RINT;
        if (client->event.version < 2) {
            YIELD; ud.errorMsg = RSTRING;
            ud.id = -1;
            ud.errorCode = -1;
        } else {
            YIELD; ud.id = RINT;
            YIELD; ud.errorCode = RINT;
            YIELD; ud.errorMsg = RSTRING;
        }

        CALLBACK(ERR_MSG);

        sk_free(ud.errorMsg);
    PEND;
}
#undef ud

#define ud client->event.data.open_order
static char receive_open_order(real_client_t *client, char *token)
{
    PBEGIN;
        ud.contract = sk_malloc(sizeof(tr_contract_t));
        tws_init_contract(ud.contract);
        ud.order = sk_malloc(sizeof(tr_order_t));
        tws_init_order(ud.order);
        ud.orderState = sk_malloc(sizeof(tr_order_state_t));
        tws_init_order_state(ud.orderState);

        client->event.version = RINT;

        YIELD; ud.order->orderId = RINT;
        if (client->event.version >= 17) {
            YIELD; ud.contract->conId = RINT;
        }
        YIELD; ud.contract->symbol = RSTRING;
        YIELD; ud.contract->secType = RSTRING;
        YIELD; ud.contract->expiry = RSTRING;
        YIELD; ud.contract->strike = RDOUBLE;
        YIELD; ud.contract->right = RSTRING;
        if (client->event.version >= 32) {
            YIELD; ud.contract->multiplier = RSTRING;
        }
        YIELD; ud.contract->exchange = RSTRING;
        YIELD; ud.contract->currency = RSTRING;
        if (client->event.version >= 2) {
            YIELD; ud.contract->localSymbol = RSTRING;
        }
        if (client->event.version >= 32) {
            YIELD; ud.contract->tradingClass = RSTRING;
        }

        /* order fileds */
        YIELD; ud.order->action = RSTRING;
        YIELD; ud.order->totalQuantity = RINT;
        YIELD; ud.order->orderType = RSTRING;
        if (client->event.version < 29) {
            YIELD; ud.order->lmtPrice = RDOUBLE;
        } else {
            YIELD; ud.order->lmtPrice = RDOUBLEMAX;
        }
        if (client->event.version < 30) {
            YIELD; ud.order->auxPrice = RDOUBLE;
        } else {
            YIELD; ud.order->auxPrice = RDOUBLEMAX;
        }
        YIELD; ud.order->tif = RSTRING;
        YIELD; ud.order->ocaGroup = RSTRING;
        YIELD; ud.order->account = RSTRING;
        YIELD; ud.order->openClose = RSTRING;
        YIELD; ud.order->origin = RINT;
        YIELD; ud.order->orderRef = RSTRING;

        if (client->event.version >= 3) {
            YIELD; ud.order->clientId = RINT;
        }

        if (client->event.version >= 4) {
            YIELD; ud.order->permId = RINT;
            YIELD; ud.order->outsideRth = RBOOL;
            YIELD; ud.order->hidden = RINT == 1;
            YIELD; ud.order->discretionaryAmt = RDOUBLE;
        }

        if (client->event.version >= 5) {
            YIELD; ud.order->goodAfterTime = RSTRING;
        }

        if (client->event.version >= 6) {
            YIELD;  /* skip deprecated sharesAllocation field */
        }

        if (client->event.version >= 7) {
            YIELD; ud.order->faGroup = RSTRING;
            YIELD; ud.order->faMethod = RSTRING;
            YIELD; ud.order->faPercentage = RSTRING;
            YIELD; ud.order->faProfile = RSTRING;
        }

        if (client->event.version >= 8) {
            YIELD; ud.order->goodTillDate = RSTRING;
        }

        if (client->event.version >= 9) {
            YIELD; ud.order->rule80A = RSTRING;
            YIELD; ud.order->percentOffset = RDOUBLEMAX;
            YIELD; ud.order->settlingFirm = RSTRING;
            YIELD; ud.order->shortSaleSlot = RINT;
            YIELD; ud.order->designatedLocation = RSTRING;
            YIELD; ud.order->exemptCode = RINT;
            YIELD; ud.order->auctionStrategy = RINT;
            YIELD; ud.order->startingPrice = RDOUBLEMAX;
            YIELD; ud.order->stockRefPrice = RDOUBLEMAX;
            YIELD; ud.order->delta = RDOUBLEMAX;
            YIELD; ud.order->stockRangeLower = RDOUBLEMAX;
            YIELD; ud.order->stockRangeUpper = RDOUBLEMAX;
            YIELD; ud.order->displaySize = RINT;
            YIELD; ud.order->blockOrder = RBOOL;
            YIELD; ud.order->sweepToFill = RBOOL;
            YIELD; ud.order->minQty = RINTMAX;
            YIELD; ud.order->ocaType = RINT;
            YIELD; ud.order->eTradeOnly = RBOOL;
            YIELD; ud.order->firmQuoteOnly = RBOOL;
            YIELD; ud.order->nbboPriceCap = RDOUBLEMAX;
        }

        if (client->event.version >= 10) {
            YIELD; ud.order->parentId = RINT;
            YIELD; ud.order->triggerMethod = RINT;
        }

        if (client->event.version >= 11) {
            YIELD; ud.order->volatility = RDOUBLEMAX;
            YIELD; ud.order->volatilityType = RINT;
            if (client->event.version == 11) {
                YIELD; ud.order->deltaNeutralOrderType = sk_strdup(((RINT == 0) ? "NONE" : "MKT"));
            } else {
                YIELD; ud.order->deltaNeutralOrderType = RSTRING;
                YIELD; ud.order->deltaNeutralAuxPrice = RDOUBLEMAX;

                if (client->event.version >= 27 && !IS_EMPTY(ud.order->deltaNeutralOrderType)) {
                    YIELD; ud.order->deltaNeutralConId = RINT;
                    YIELD; ud.order->deltaNeutralSettlingFirm = RSTRING;
                    YIELD; ud.order->deltaNeutralClearingAccount = RSTRING;
                    YIELD; ud.order->deltaNeutralClearingIntent = RSTRING;
                }

                if (client->event.version >= 31 && !IS_EMPTY(ud.order->deltaNeutralOrderType)) {
                    YIELD; ud.order->deltaNeutralOpenClose = RSTRING;
                    YIELD; ud.order->deltaNeutralShortSale = RBOOL;
                    YIELD; ud.order->deltaNeutralShortSaleSlot = RINT;
                    YIELD; ud.order->deltaNeutralDesignatedLocation = RSTRING;
                }
            }
            YIELD; ud.order->continuousUpdate = RINT;
            YIELD; ud.order->referencePriceType = RINT;
        }

        if (client->event.version >= 13) {
            YIELD; ud.order->trailStopPrice = RDOUBLEMAX;
        }

        if (client->event.version >= 30) {
            YIELD; ud.order->trailingPercent = RDOUBLEMAX;
        }

        if (client->event.version >= 14) {
            YIELD; ud.order->basisPoints = RDOUBLEMAX;
            YIELD; ud.order->basisPointsType = RINTMAX;
            YIELD; ud.contract->comboLegsDescrip = RSTRING;
        }

        if (client->event.version >= 29) {
            YIELD; ud.contract->comboLegsCount = RINT;
            if (ud.contract->comboLegsCount > 0) {
                ud.contract->comboLegs = (tr_comboleg_t *)sk_calloc(sizeof(tr_comboleg_t),
                                                                   ud.contract->comboLegsCount);
                for (client->event.j = 0; client->event.j < ud.contract->comboLegsCount; client->event.j++) {
                    YIELD; ud.contract->comboLegs[client->event.j].conId = RINT;
                    YIELD; ud.contract->comboLegs[client->event.j].ratio = RINT;
                    YIELD; ud.contract->comboLegs[client->event.j].action = RSTRING;
                    YIELD; ud.contract->comboLegs[client->event.j].exchange = RSTRING;
                    YIELD; ud.contract->comboLegs[client->event.j].openClose = RINT;
                    YIELD; ud.contract->comboLegs[client->event.j].shortSaleSlot = RINT;
                    YIELD; ud.contract->comboLegs[client->event.j].designatedLocation = RSTRING;
                    YIELD; ud.contract->comboLegs[client->event.j].exemptCode = RINT;
                }
            }

            YIELD; ud.order->orderComboLegsCount = RINT;
            if (ud.order->orderComboLegsCount > 0) {
                ud.order->orderComboLegs = (tr_order_combo_leg_t *)sk_calloc (sizeof(tr_order_combo_leg_t),
                                                                          ud.order->orderComboLegsCount);
                for (client->event.j = 0; client->event.j < ud.order->orderComboLegsCount; client->event.j++) {
                    YIELD; ud.order->orderComboLegs[client->event.j].price = RDOUBLEMAX;
                }
            }
        }

        if (client->event.version >= 26) {
            YIELD; ud.order->smartComboRoutingParamsCount = RINT;
            if (ud.order->smartComboRoutingParamsCount > 0) {
                ud.order->smartComboRoutingParams  = (tr_tag_value_t*)sk_calloc(sizeof(tr_tag_value_t),
                                         ud.order->smartComboRoutingParamsCount);
                for (client->event.j = 0; client->event.j < ud.order->smartComboRoutingParamsCount; client->event.j++) {
                    YIELD; ud.order->smartComboRoutingParams[client->event.j].tag = RSTRING;
                    YIELD; ud.order->smartComboRoutingParams[client->event.j].val = RSTRING;
                }
            }
        }

        if (client->event.version >= 15) {
            if (client->event.version >= 20) {
                YIELD; ud.order->scaleInitLevelSize = RINTMAX;
                YIELD; ud.order->scaleSubsLevelSize = RINTMAX;
            } else {
                YIELD;  /* int notSuppScaleNumComponents */
                YIELD; ud.order->scaleInitLevelSize = RINTMAX;
            }
            YIELD; ud.order->scalePriceIncrement = RDOUBLEMAX;
        }

        if (client->event.version >= 28 && ud.order->scalePriceIncrement > 0.0 &&
                ud.order->scalePriceIncrement != DBL_MAX) {
            YIELD; ud.order->scalePriceAdjustValue = RDOUBLEMAX;
            YIELD; ud.order->scalePriceAdjustInterval = RINTMAX;
            YIELD; ud.order->scaleProfitOffset = RDOUBLEMAX;
            YIELD; ud.order->scaleAutoReset = RBOOL;
            YIELD; ud.order->scaleInitPosition = RINTMAX;
            YIELD; ud.order->scaleInitFillQty = RINTMAX;
            YIELD; ud.order->scaleRandomPercent = RBOOL;
        }

        if (client->event.version >= 24) {
            YIELD; ud.order->hedgeType = RSTRING;
            if (!IS_EMPTY(ud.order->hedgeType))
                YIELD; ud.order->hedgeParam = RSTRING;
        }

        if (client->event.version >= 25) {
            YIELD; ud.order->optOutSmartRouting = RBOOL;
        }

        if (client->event.version >= 19) {
            YIELD; ud.order->clearingAccount = RSTRING;
            YIELD; ud.order->clearingIntent = RSTRING;
        }

        if (client->event.version >= 22) {
            YIELD; ud.order->notHeld = RBOOL;
        }

        if (client->event.version >= 20) {
            YIELD; if (RBOOL) {
                ud.contract->underComp = sk_malloc(sizeof(tr_under_comp_t));
                YIELD; ud.contract->underComp->conId = RINT;
                YIELD; ud.contract->underComp->delta = RDOUBLE;
                YIELD; ud.contract->underComp->price = RDOUBLE;
            }
        }

        if (client->event.version >= 21) {
            YIELD; ud.order->algoStrategy = RSTRING;
            if (!IS_EMPTY(ud.order->algoStrategy)) {
                YIELD; ud.order->algoParamsCount = RINT;
                if (ud.order->algoParamsCount > 0) {
                    ud.order->algoParams = sk_calloc(sizeof(tr_tag_value_t),
                                                   ud.order->algoParamsCount);
                    for (client->event.j = 0; client->event.j < ud.order->algoParamsCount; client->event.j++) {
                        YIELD; ud.order->algoParams[client->event.j].tag = RSTRING;
                        YIELD; ud.order->algoParams[client->event.j].val = RSTRING;
                    }
                }
            }
        }

        if (client->event.version >= 16) {
            YIELD; ud.order->whatIf = RBOOL;
            YIELD; ud.orderState->status = RSTRING;
            YIELD; ud.orderState->initMargin = RSTRING;
            YIELD; ud.orderState->maintMargin = RSTRING;
            YIELD; ud.orderState->equityWithLoan = RSTRING;
            YIELD; ud.orderState->commission = RDOUBLEMAX;
            YIELD; ud.orderState->minCommission = RDOUBLEMAX;
            YIELD; ud.orderState->maxCommission = RDOUBLEMAX;
            YIELD; ud.orderState->commissionCurrency = RSTRING;
            YIELD; ud.orderState->warningText = RSTRING;
        }
        CALLBACK(OPEN_ORDER);
        /* free */
        tws_destroy_contract(ud.contract);
        tws_destroy_order(ud.order);
        tws_destroy_order_state(ud.orderState);
        sk_free(ud.contract);
        sk_free(ud.order);
        sk_free(ud.orderState);
    PEND;
}
#undef ud

#define ud client->event.data.next_valid_id
static char receive_next_valid_id(real_client_t *client, char *token)
{
    PBEGIN;
        /* ignore version */
        YIELD; ud.orderId = RINT;
        CALLBACK(NEXT_VALID_ID);
    PEND;
}
#undef ud

#define ud client->event.data.scanner_data
static char receive_scanner_data(real_client_t *client, char *token)
{
    PBEGIN;
        client->event.version = RINT;
        YIELD; ud.tickerId = RINT;
        YIELD; ud.numberOfElements = RINT;
        if (ud.numberOfElements > 0) {
            ud.elements = sk_calloc(sizeof(event_scanner_data_item_t), ud.numberOfElements);
            for (client->event.j = 0; client->event.j < ud.numberOfElements; client->event.j++) {
                tws_init_contract_details(ud.elements[client->event.j].contractDetails);
                YIELD; ud.elements[client->event.j].rank = RINT;
                if (client->event.version >= 3) {
                    YIELD; ud.elements[client->event.j].contractDetails->summary.conId = RINT;
                }
                YIELD; ud.elements[client->event.j].contractDetails->summary.symbol = RSTRING;
                YIELD; ud.elements[client->event.j].contractDetails->summary.secType = RSTRING;
                YIELD; ud.elements[client->event.j].contractDetails->summary.expiry = RSTRING;
                YIELD; ud.elements[client->event.j].contractDetails->summary.strike = RDOUBLE;
                YIELD; ud.elements[client->event.j].contractDetails->summary.right = RSTRING;
                YIELD; ud.elements[client->event.j].contractDetails->summary.exchange = RSTRING;
                YIELD; ud.elements[client->event.j].contractDetails->summary.currency = RSTRING;
                YIELD; ud.elements[client->event.j].contractDetails->summary.localSymbol = RSTRING;
                YIELD; ud.elements[client->event.j].contractDetails->marketName = RSTRING;
                YIELD; ud.elements[client->event.j].contractDetails->summary.tradingClass = RSTRING;
                YIELD; ud.elements[client->event.j].distance = RSTRING;
                YIELD; ud.elements[client->event.j].benchmark = RSTRING;
                YIELD; ud.elements[client->event.j].projection = RSTRING;
                ud.elements[client->event.j].legsStr = NULL;
                if (client->event.version >= 2) {
                    YIELD; ud.elements[client->event.j].legsStr = RSTRING;
                }
            }
        }

        CALLBACK(SCANNER_DATA);
        /* free */
        for (client->event.j = 0; client->event.j < ud.numberOfElements; client->event.j++) {
            tws_destroy_contract_details(ud.elements[client->event.j].contractDetails);
            sk_free(ud.elements[client->event.j].distance);
            sk_free(ud.elements[client->event.j].benchmark);
            sk_free(ud.elements[client->event.j].projection);
            sk_free(ud.elements[client->event.j].legsStr);
        }
        sk_free(ud.elements);
    PEND;
}
#undef ud

#define ud client->event.data.contract_data
static char receive_contract_data(real_client_t *client, char *token)
{
    PBEGIN;
        ud.contractDetails = sk_malloc(sizeof(tr_contract_details_t));
        tws_init_contract_details(ud.contractDetails);

        client->event.version = RINT;

        ud.reqId = -1;
        if (client->event.version >= 3) {
            YIELD; ud.reqId = RINT;
        }
        YIELD; ud.contractDetails->summary.symbol = RSTRING;
        YIELD; ud.contractDetails->summary.secType = RSTRING;
        YIELD; ud.contractDetails->summary.expiry = RSTRING;
        YIELD; ud.contractDetails->summary.strike = RDOUBLE;
        YIELD; ud.contractDetails->summary.right = RSTRING;
        YIELD; ud.contractDetails->summary.exchange = RSTRING;
        YIELD; ud.contractDetails->summary.currency = RSTRING;
        YIELD; ud.contractDetails->summary.localSymbol = RSTRING;
        YIELD; ud.contractDetails->marketName = RSTRING;
        YIELD; ud.contractDetails->summary.tradingClass = RSTRING;
        YIELD; ud.contractDetails->summary.conId = RINT;
        YIELD; ud.contractDetails->minTick = RDOUBLE;
        YIELD; ud.contractDetails->summary.multiplier = RSTRING;
        YIELD; ud.contractDetails->orderTypes = RSTRING;
        YIELD; ud.contractDetails->validExchanges = RSTRING;
        if (client->event.version >= 2) {
            YIELD; ud.contractDetails->priceMagnifier = RINT;
        }

        if (client->event.version >= 4) {
            YIELD; ud.contractDetails->underConId = RINT;
        }

        if (client->event.version >= 5) {
            YIELD; ud.contractDetails->longName = RSTRING;
            YIELD; ud.contractDetails->summary.primaryExch = RSTRING;
        }

        if (client->event.version >= 6) {
            YIELD; ud.contractDetails->contractMonth = RSTRING;
            YIELD; ud.contractDetails->industry = RSTRING;
            YIELD; ud.contractDetails->category = RSTRING;
            YIELD; ud.contractDetails->subcategory = RSTRING;
            YIELD; ud.contractDetails->timeZoneId = RSTRING;
            YIELD; ud.contractDetails->tradingHours = RSTRING;
            YIELD; ud.contractDetails->liquidHours = RSTRING;
        }

        if (client->event.version >= 8) {
            YIELD; ud.contractDetails->evRule = RSTRING;
            YIELD; ud.contractDetails->evMultiplier = RDOUBLE;
        }

        if (client->event.version >= 7) {
            YIELD; ud.contractDetails->secIdListCount = RINT;
            if (ud.contractDetails->secIdListCount > 0) {
                ud.contractDetails->secIdList = sk_calloc(sizeof(tr_tag_value_t),
                                                        ud.contractDetails->secIdListCount);
                for (client->event.j = 0; client->event.j < ud.contractDetails->secIdListCount; client->event.j++) {
                    YIELD; ud.contractDetails->secIdList[client->event.j].tag = RSTRING;
                    YIELD; ud.contractDetails->secIdList[client->event.j].val = RSTRING;
                }
            }
        }

        CALLBACK(CONTRACT_DATA);

        tws_destroy_contract_details(ud.contractDetails);
        sk_free(ud.contractDetails);
    PEND;
}
#undef ud

#define ud client->event.data.bond_contract_data
static char receive_bond_contract_data(real_client_t *client, char *token)
{
    PBEGIN;
        ud.contractDetails = sk_malloc(sizeof(tr_contract_details_t));
        tws_init_contract_details(ud.contractDetails);

        client->event.version = RINT;

        ud.reqId = -1;
        if (client->event.version >= 3) {
            YIELD; ud.reqId = RINT;
        }

        YIELD; ud.contractDetails->summary.symbol = RSTRING;
        YIELD; ud.contractDetails->summary.secType = RSTRING;
        YIELD; ud.contractDetails->cusip = RSTRING;
        YIELD; ud.contractDetails->coupon = RDOUBLE;
        YIELD; ud.contractDetails->maturity = RSTRING;
        YIELD; ud.contractDetails->issueDate = RSTRING;
        YIELD; ud.contractDetails->ratings = RSTRING;
        YIELD; ud.contractDetails->bondType = RSTRING;
        YIELD; ud.contractDetails->couponType = RSTRING;
        YIELD; ud.contractDetails->convertible = RBOOL;
        YIELD; ud.contractDetails->callable = RBOOL;
        YIELD; ud.contractDetails->putable = RBOOL;
        YIELD; ud.contractDetails->descAppend = RSTRING;
        YIELD; ud.contractDetails->summary.exchange = RSTRING;
        YIELD; ud.contractDetails->summary.currency = RSTRING;
        YIELD; ud.contractDetails->marketName = RSTRING;
        YIELD; ud.contractDetails->summary.tradingClass = RSTRING;
        YIELD; ud.contractDetails->summary.conId = RINT;
        YIELD; ud.contractDetails->minTick = RDOUBLE;
        YIELD; ud.contractDetails->orderTypes = RSTRING;
        YIELD; ud.contractDetails->validExchanges = RSTRING;

        if (client->event.version >= 2) {
            YIELD; ud.contractDetails->nextOptionDate = RSTRING;
            YIELD; ud.contractDetails->nextOptionType = RSTRING;
            YIELD; ud.contractDetails->nextOptionPartial = RBOOL;
            YIELD; ud.contractDetails->notes = RSTRING;
        }

        if (client->event.version >= 4) {
            YIELD; ud.contractDetails->longName = RSTRING;
        }

        if (client->event.version >= 6) {
            YIELD; ud.contractDetails->evRule = RSTRING;
            YIELD; ud.contractDetails->evMultiplier = RDOUBLE;
        }

        if (client->event.version >= 5) {
            YIELD; ud.contractDetails->secIdListCount = RINT;
            if (ud.contractDetails->secIdListCount > 0) {
                ud.contractDetails->secIdList = sk_calloc(sizeof(tr_tag_value_t),
                                                        ud.contractDetails->secIdListCount);
                for (client->event.j = 0; client->event.j < ud.contractDetails->secIdListCount; client->event.j++) {
                    ud.contractDetails->secIdList[client->event.j].tag = RSTRING;
                    ud.contractDetails->secIdList[client->event.j].val = RSTRING;
                }
            }
        }

        CALLBACK(BOND_CONTRACT_DATA);

        tws_destroy_contract_details(ud.contractDetails);
        sk_free(ud.contractDetails);
    PEND;
}
#undef ud

#define ud client->event.data.execution_data
static char receive_execution_data(real_client_t *client, char *token)
{
    PBEGIN;
        ud.contract = sk_malloc(sizeof(tr_contract_t));
        tws_init_contract(ud.contract);
        ud.exec = sk_malloc(sizeof(tr_execution_t));
        tws_init_execution(ud.exec);

        client->event.version = RINT;
        ud.reqId = -1;
        if (client->event.version >= 7) {
            YIELD; ud.reqId = RINT;
        }

        YIELD; ud.exec->orderId = RINT;

        if (client->event.version >= 5) {
            YIELD; ud.contract->conId = RINT;
        }

        YIELD; ud.contract->symbol = RSTRING;
        YIELD; ud.contract->secType = RSTRING;
        YIELD; ud.contract->expiry = RSTRING;
        YIELD; ud.contract->strike = RDOUBLE;
        YIELD; ud.contract->right = RSTRING;

        if (client->event.version >= 9) {
            YIELD; ud.contract->multiplier = RSTRING;
        }

        YIELD; ud.contract->exchange = RSTRING;
        YIELD; ud.contract->currency = RSTRING;
        YIELD; ud.contract->localSymbol = RSTRING;

        if (client->event.version >= 10) {
            YIELD; ud.contract->tradingClass = RSTRING;
        }

        YIELD; ud.exec->execId = RSTRING;
        YIELD; ud.exec->time = RSTRING;
        YIELD; ud.exec->acctNumber = RSTRING;
        YIELD; ud.exec->exchange = RSTRING;
        YIELD; ud.exec->side = RSTRING;
        YIELD; ud.exec->shares = RINT;
        YIELD; ud.exec->price = RDOUBLE;

        if (client->event.version >= 2) {
            YIELD; ud.exec->permId = RINT;
        }

        if (client->event.version >= 3) {
            YIELD; ud.exec->clientId = RINT;
        }

        if (client->event.version >= 4) {
            YIELD; ud.exec->liquidation = RINT;
        }

        if (client->event.version >= 6) {
            YIELD; ud.exec->cumQty = RINT;
            YIELD; ud.exec->avgPrice = RDOUBLE;
        }

        if (client->event.version >= 8) {
            YIELD; ud.exec->orderRef = RSTRING;
        }

        if (client->event.version >= 9) {
            YIELD; ud.exec->evRule = RSTRING;
            YIELD; ud.exec->evMultiplier = RDOUBLE;
        }

        CALLBACK(EXECUTION_DATA);
        /* free */
        tws_destroy_execution(ud.exec);
        tws_destroy_contract(ud.contract);
        sk_free(ud.exec);
        sk_free(ud.contract);

    PEND;
}
#undef ud

#define ud client->event.data.market_depth
static char receive_market_depth(real_client_t *client, char *token)
{
    PBEGIN;
        client->event.version = RINT;
        YIELD; ud.id = RINT;
        YIELD; ud.position = RINT;
        YIELD; ud.operation = RINT;
        YIELD; ud.side = RINT;
        YIELD; ud.price = RDOUBLE;
        YIELD; ud.size = RINT;

        CALLBACK(MARKET_DEPTH);
    PEND;
}
#undef ud

#define ud client->event.data.market_depth_l2
static char receive_market_depth_l2(real_client_t *client, char *token)
{
    PBEGIN;
        client->event.version = RINT;
        YIELD; ud.id = RINT;
        YIELD; ud.position = RINT;
        YIELD; ud.marketMaker = RSTRING;
        YIELD; ud.operation = RINT;
        YIELD; ud.side = RINT;
        YIELD; ud.price = RDOUBLE;
        YIELD; ud.size = RINT;

        CALLBACK(MARKET_DEPTH_L2);
    PEND;
}
#undef ud

#define ud client->event.data.news_bulletins
static char receive_news_bulletions(real_client_t *client, char *token)
{
    PBEGIN;
        /* ignore version */
        YIELD; ud.newsMsgId = RINT;
        YIELD; ud.newsMsgType = RINT;
        YIELD; ud.newsMessage = RSTRING;
        YIELD; ud.originationgExch = RSTRING;

        CALLBACK(NEWS_BULLETINS);

        sk_free(ud.originationgExch);
        sk_free(ud.newsMessage);
    PEND;
}
#undef ud

#define ud client->event.data.managed_accts
static char receive_managed_accts(real_client_t *client, char *token)
{
    PBEGIN;
        /* ignore version */
        YIELD;  ud.acctList = RSTRING;

        CALLBACK(MANAGED_ACCTS);

        sk_free(ud.acctList);
    PEND;
}
#undef ud

#define ud client->event.data.receive_fa
static char receive_fa(real_client_t *client, char *token)
{
    PBEGIN;
        /* ignore version */
        YIELD;  ud.faDataType = RINT;
        YIELD;  ud.xml = RSTRING;

        CALLBACK(RECEIVE_FA);

        sk_free(ud.xml);
    PEND;
}
#undef ud

#define ud client->event.data.historical_data
static char receive_historical_data(real_client_t *client, char *token)
{
    PBEGIN;
        client->event.version = RINT;
        YIELD; ud.reqId = RINT;

        if (client->event.version >= 2) {
            YIELD; ud.startDateStr = RSTRING;
            YIELD; ud.endDateStr = RSTRING;
        }
        YIELD; ud.itemCount = RINT;
        if (ud.itemCount > 0) {
            ud.items = sk_calloc(sizeof(event_historical_data_item_t), ud.itemCount);
            for (client->event.j = 0; client->event.j < ud.itemCount; client->event.j++) {
                YIELD; ud.items[client->event.j].date = RSTRING;
                YIELD; ud.items[client->event.j].open = RDOUBLE;
                YIELD; ud.items[client->event.j].high = RDOUBLE;
                YIELD; ud.items[client->event.j].low = RDOUBLE;
                YIELD; ud.items[client->event.j].close = RDOUBLE;
                YIELD; ud.items[client->event.j].volume = RINT;
                YIELD; ud.items[client->event.j].WAP = RDOUBLE;
                YIELD; ud.items[client->event.j].hasGaps = RBOOL;
                ud.items[client->event.j].barCount = -1;
                if (client->event.version >= 3) {
                    YIELD; ud.items[client->event.j].barCount = RINT;
                }
            }
        }

        CALLBACK(HISTORICAL_DATA);

        /* free */
        sk_free(ud.startDateStr);
        sk_free(ud.endDateStr);
        for (client->event.j = 0; client->event.j < ud.itemCount; client->event.j++) {
            sk_free(ud.items[client->event.j].date);
        }
        sk_free(ud.items);

    PEND;
}
#undef ud

#define ud client->event.data.scanner_parameters
static char receive_scanner_parameters(real_client_t *client, char *token)
{
    PBEGIN;
        /* ignore version */
        YIELD; ud.xml = RSTRING;

        CALLBACK(SCANNER_PARAMETERS);

        sk_free(ud.xml);
    PEND;
}
#undef ud

#define ud client->event.data.current_time
static char receive_current_time(real_client_t *client, char *token)
{
    PBEGIN;
        /* ignore version */
        YIELD; ud.time = RLONG;

        CALLBACK(CURRENT_TIME);
    PEND;
}
#undef ud

#define ud client->event.data.realtime_bars
static char receive_realtime_bars(real_client_t *client, char *token)
{
    PBEGIN;
        /* ignore version */
        YIELD; ud.reqId = RINT;
        YIELD; ud.time = RLONG;
        YIELD; ud.open = RDOUBLE;
        YIELD; ud.high = RDOUBLE;
        YIELD; ud.low = RDOUBLE;
        YIELD; ud.close = RDOUBLE;
        YIELD; ud.volume = RLONG;
        YIELD; ud.wap = RDOUBLE;
        YIELD; ud.count = RINT;

        CALLBACK(REAL_TIME_BARS);
    PEND;
}
#undef ud

#define ud client->event.data.fundamental_data
static char receive_fundamental_data(real_client_t *client, char *token)
{
    PBEGIN;
        /* ignore version */
        YIELD; ud.reqId = RINT;
        YIELD; ud.data = RSTRING;

        CALLBACK(FUNDAMENTAL_DATA);

        sk_free(ud.data);
    PEND;
}
#undef ud

#define ud client->event.data.contract_data_end
static char receive_contract_data_end(real_client_t *client, char *token)
{
    PBEGIN;
        /* ignore version */
        YIELD; ud.reqId = RINT;

        CALLBACK(CONTRACT_DATA_END);
    PEND;
}
#undef ud

#define ud client->event.data.open_order_end
static char receive_open_order_end(real_client_t *client, char *token)
{
    UNUSED(token);
    NBEGIN
        /* ignore version */
        CALLBACK(OPEN_ORDER_END);
    NEND;
}
#undef ud

#define ud client->event.data.acct_download_end
static char receive_acct_download_end(real_client_t *client, char *token)
{
    PBEGIN;
        /* ignore version */
        YIELD; ud.accountName = RSTRING;

        CALLBACK(ACCT_DOWNLOAD_END);

        sk_free(ud.accountName);
    PEND;
}
#undef ud

#define ud client->event.data.execution_data_end
static char receive_execution_data_end(real_client_t *client, char *token)
{
    PBEGIN;
        /* ignore version */
        YIELD; ud.reqId = RINT;

        CALLBACK(EXECUTION_DATA_END);
    PEND;
}
#undef ud

#define ud client->event.data.delta_neutral_validation
static char receive_delta_neutral_validation(real_client_t *client, char *token)
{
    PBEGIN;
        /* ignore version */
        YIELD; ud.reqId = RINT;
        YIELD; ud.underComp.conId = RINT;
        YIELD; ud.underComp.delta = RDOUBLE;
        YIELD; ud.underComp.price = RDOUBLE;

        CALLBACK(DELTA_NEUTRAL_VALIDATION);
    PEND;
}
#undef ud

#define ud client->event.data.tick_snapshot_end
static char receive_tick_snapshot_end(real_client_t *client, char *token)
{
    PBEGIN;
        /* ignore version */
        YIELD; ud.reqId = RINT;

        CALLBACK(TICK_SNAPSHOT_END);
    PEND;
}
#undef ud

#define ud client->event.data.market_data_type
static char receive_market_data_type(real_client_t *client, char *token)
{
    PBEGIN;
        /* ignore version */
        YIELD; ud.reqId = RINT;
        YIELD; ud.marketDataType = RINT;

        CALLBACK(MARKET_DATA_TYPE);
    PEND;
}
#undef ud

#define ud client->event.data.commission_report
static char receive_commission_report(real_client_t *client, char *token)
{
    PBEGIN;
        /* ignore version */
        YIELD; ud.report.execId = RSTRING;
        YIELD; ud.report.commission = RDOUBLE;
        YIELD; ud.report.currency = RSTRING;
        YIELD; ud.report.realizedPNL = RDOUBLE;
        YIELD; ud.report.yield = RDOUBLE;
        YIELD; ud.report.yieldRedemptionDate = RINT;

        CALLBACK(COMMISSION_REPORT);

        sk_free(ud.report.execId);
        sk_free(ud.report.currency);
    PEND;
}
#undef ud

#define ud client->event.data.verify_message_api
static char receive_verify_message_api(real_client_t *client, char *token)
{
    PBEGIN;
        /* ignore version */
        YIELD; ud.apiData = RSTRING;

        CALLBACK(VERIFY_MESSAGE_API);

        sk_free(ud.apiData);
    PEND;
}
#undef ud

#define ud client->event.data.verify_completed
static char receive_verify_completed(real_client_t *client, char *token)
{
    PBEGIN;
        /* ignore version */
        YIELD;
        if (ascii_strcasecmp(token, "true") == 0) {
            ud.isSuccessful = 1;
        } else {
            ud.isSuccessful = 0;
        }

        YIELD; ud.errorText = RSTRING;
        if (ud.isSuccessful)
            tws_client_startAPI(client);

        CALLBACK(VERIFY_COMPLETED);

        sk_free(ud.errorText);
    PEND;
}
#undef ud

#define ud client->event.data.display_group_list
static char receive_display_group_list(real_client_t *client, char *token)
{
    PBEGIN;
        /* ignore version */
        YIELD; ud.reqId = RINT;
        YIELD; ud.groups = RSTRING;

        CALLBACK(DISPLAY_GROUP_LIST);

        sk_free(ud.groups);
    PEND;
}
#undef ud

#define ud client->event.data.display_group_updated
static char receive_display_group_updated(real_client_t *client, char *token)
{
    PBEGIN;
        /* ignore version */
        YIELD; ud.reqId = RINT;
        YIELD; ud.contractInfo = RSTRING;

        CALLBACK(DISPLAY_GROUP_UPDATED);

        sk_free(ud.contractInfo);
    PEND;
}
#undef ud

static void decode_response_package(real_client_t *client, char *token)
{
    int pt_value = 0;
    switch (client->cmd) {
    case 0:
        client->cmd = atoi(token);
#ifndef NDEBUG
        client->packet = token;
#endif
        break;
    case TICK_PRICE:
        pt_value = receive_tick_price(client, token);
        break;
    case TICK_SIZE:
        pt_value = receive_tick_size(client, token);
        break;
    case POSITION:
        pt_value = receive_position(client, token);
        break;
    case POSITION_END:
        pt_value = receive_position_end(client, token);
        break;
    case ACCOUNT_SUMMARY:
        pt_value = receive_account_summary(client, token);
        break;
    case ACCOUNT_SUMMARY_END:
        pt_value = receive_account_summary_end(client, token);
        break;
    case TICK_OPTION_COMPUTATION:
        pt_value = receive_tick_option_computation(client, token);
        break;
    case TICK_GENERIC:
        pt_value = receive_tick_generic(client, token);
        break;
    case TICK_STRING:
        pt_value = receive_tick_string(client, token);
        break;
    case TICK_EFP:
        pt_value = receive_tick_efp(client, token);
        break;
    case ORDER_STATUS:
        pt_value = receive_order_status(client, token);
        break;
    case ACCT_VALUE:
        pt_value = receive_acct_value(client, token);
        break;
    case PORTFOLIO_VALUE:
        pt_value = receive_portfolio_value(client, token);
        break;
    case ACCT_UPDATE_TIME:
        pt_value = receive_acct_update_time(client, token);
        break;
    case ERR_MSG:
        pt_value = receive_err_msg(client, token);
        break;
    case OPEN_ORDER:
        pt_value = receive_open_order(client, token);
        break;
    case NEXT_VALID_ID:
        pt_value = receive_next_valid_id(client, token);
        break;
    case SCANNER_DATA:
        pt_value = receive_scanner_data(client, token);
        break;
    case CONTRACT_DATA:
        pt_value = receive_contract_data(client, token);
        break;
    case BOND_CONTRACT_DATA:
        pt_value = receive_bond_contract_data(client, token);
        break;
    case EXECUTION_DATA:
        pt_value = receive_execution_data(client, token);
        break;
    case MARKET_DEPTH:
        pt_value = receive_market_depth(client, token);
        break;
    case MARKET_DEPTH_L2:
        pt_value = receive_market_depth_l2(client, token);
        break;
    case NEWS_BULLETINS:
        pt_value = receive_news_bulletions(client, token);
        break;
    case MANAGED_ACCTS:
        pt_value = receive_managed_accts(client, token);
        break;
    case RECEIVE_FA:
        pt_value = receive_fa(client, token);
        break;
    case HISTORICAL_DATA:
        pt_value = receive_historical_data(client, token);
        break;
    case SCANNER_PARAMETERS:
        pt_value = receive_scanner_parameters(client, token);
        break;
    case CURRENT_TIME:
        pt_value = receive_current_time(client, token);
        break;
    case REAL_TIME_BARS:
        pt_value = receive_realtime_bars(client, token);
        break;
    case FUNDAMENTAL_DATA:
        pt_value = receive_fundamental_data(client, token);
        break;
    case CONTRACT_DATA_END:
        pt_value = receive_contract_data_end(client, token);
        break;
    case OPEN_ORDER_END:
        pt_value = receive_open_order_end(client, token);
        break;
    case ACCT_DOWNLOAD_END:
        pt_value = receive_acct_download_end(client, token);
        break;
    case EXECUTION_DATA_END:
        pt_value = receive_execution_data_end(client, token);
        break;
    case DELTA_NEUTRAL_VALIDATION:
        pt_value = receive_delta_neutral_validation(client, token);
        break;
    case TICK_SNAPSHOT_END:
        pt_value = receive_tick_snapshot_end(client, token);
        break;
    case MARKET_DATA_TYPE:
        pt_value = receive_market_data_type(client, token);
        break;
    case COMMISSION_REPORT:
        pt_value = receive_commission_report(client, token);
        break;
    case VERIFY_MESSAGE_API:
        pt_value = receive_verify_message_api(client, token);
        break;
    case VERIFY_COMPLETED:
        pt_value = receive_verify_completed(client, token);
        break;
    case DISPLAY_GROUP_LIST:
        pt_value = receive_display_group_list(client, token);
        break;
    case DISPLAY_GROUP_UPDATED:
        pt_value = receive_display_group_updated(client, token);
        break;
    default:
        LOG_ERR("Unknow Tws Commnad [%d]...disconnect\n", client->cmd);
        exit(0);
    }

    /* pt_value == PT_ENDED means the packet decode finished, so we wait for next commnad */
    if (PT_ENDED == pt_value) {
        client->cmd = 0;
#ifndef NDEBUG
        hexdump("[[", client->packet, client->p - client->packet);
        client->packet = client->p;
#endif
    }
}

void tws_decode_message(real_client_t *client, char *token)
{
if (client->state < TWS_ST_AUTHED)
    decode_login_package(client, token);
else
    decode_response_package(client, token);
}

void tws_decode_reset(real_client_t *client)
{
    client->rstart = 0;
    client->rlen = 0;
    client->p = client->rbuf;
    PT_INIT(&client->pt);
    client->cmd = 0;
}

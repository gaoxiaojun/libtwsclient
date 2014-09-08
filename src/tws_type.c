#include <limits.h>
#include <float.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <twsclient/tws_type.h>

#define __TWS_INTERNAL__
#include "tws_private.h"

/* Data API */

void tws_init_contract(tr_contract_t *c)
{
    memset(c, 0, sizeof * c);
}

void tws_destroy_contract(tr_contract_t *c)
{
    sk_free(c->symbol);
    sk_free(c->secType);
    sk_free(c->expiry);
    sk_free(c->right);
    sk_free(c->multiplier);
    sk_free(c->exchange);
    sk_free(c->currency);
    sk_free(c->localSymbol);
    sk_free(c->tradingClass);
    sk_free(c->primaryExch);
    sk_free(c->secIdType);
    sk_free(c->secId);
    sk_free(c->comboLegsDescrip);

    if (c->comboLegs && c->comboLegsCount) {
        for (int i = 0; i < c->comboLegsCount; i++) {
            sk_free(c->comboLegs[i].action);
            sk_free(c->comboLegs[i].exchange);
            sk_free(c->comboLegs[i].designatedLocation);
        }
        sk_free(c->comboLegs);
    }

    sk_free(c->underComp);
}

void tws_init_order(tr_order_t *o)
{
    memset(o, 0, sizeof * o);

    o->lmtPrice = DBL_MAX;
    o->auxPrice = DBL_MAX;

    o->outsideRth = false;
    o->openClose = sk_strdup("O");
    o->origin = CUSTOMER;
    o->transmit = true;
    o->exemptCode = -1;
    o->minQty = INT_MAX;
    o->percentOffset = DBL_MAX;
    o->nbboPriceCap = DBL_MAX;
    o->optOutSmartRouting = false;
    o->startingPrice = DBL_MAX;
    o->stockRefPrice = DBL_MAX;
    o->delta = DBL_MAX;
    o->stockRangeLower = DBL_MAX;
    o->stockRangeUpper = DBL_MAX;
    o->volatility = DBL_MAX;
    o->volatilityType = INT_MAX;
    o->deltaNeutralAuxPrice = DBL_MAX;
    o->deltaNeutralConId = 0;
    o->deltaNeutralShortSale = false;
    o->deltaNeutralShortSaleSlot = 0;
    o->referencePriceType = INT_MAX;
    o->trailStopPrice = DBL_MAX;
    o->trailingPercent = DBL_MAX;
    o->basisPoints = DBL_MAX;
    o->basisPointsType = INT_MAX;
    o->scaleInitLevelSize = INT_MAX;
    o->scaleSubsLevelSize = INT_MAX;
    o->scalePriceIncrement = DBL_MAX;
    o->scalePriceAdjustValue = DBL_MAX;
    o->scalePriceAdjustInterval = INT_MAX;
    o->scaleProfitOffset = DBL_MAX;
    o->scaleAutoReset = false;
    o->scaleInitPosition = INT_MAX;
    o->scaleInitFillQty = INT_MAX;
    o->scaleRandomPercent = false;
    o->whatIf = false;
    o->notHeld = false;
}

void tws_destroy_order(tr_order_t *o)
{
    sk_free(o->action);
    sk_free(o->orderType);
    sk_free(o->tif);
    sk_free(o->activeStartTime);
    sk_free(o->activeStopTime);
    sk_free(o->ocaGroup);
    sk_free(o->orderRef);
    sk_free(o->goodAfterTime);
    sk_free(o->goodTillDate);
    sk_free(o->rule80A);
    sk_free(o->faGroup);
    sk_free(o->faProfile);
    sk_free(o->faPercentage);
    sk_free(o->faMethod);
    sk_free(o->openClose);
    sk_free(o->designatedLocation);

    sk_free(o->deltaNeutralOrderType);
    sk_free(o->deltaNeutralSettlingFirm);
    sk_free(o->deltaNeutralClearingAccount);
    sk_free(o->deltaNeutralClearingIntent);
    sk_free(o->deltaNeutralOpenClose);
    sk_free(o->deltaNeutralDesignatedLocation);
    sk_free(o->scaleTable);
    sk_free(o->hedgeType);
    sk_free(o->hedgeParam);
    sk_free(o->account);
    sk_free(o->settlingFirm);
    sk_free(o->clearingAccount);
    sk_free(o->clearingIntent);
    sk_free(o->algoStrategy);

    if (o->algoParams && o->algoParamsCount) {
        for (int i = 0; i < o->algoParamsCount; i++) {
            sk_free(o->algoParams[i].tag);
            sk_free(o->algoParams[i].val);
        }

        sk_free(o->algoParams);
    }

    if (o->smartComboRoutingParams && o->smartComboRoutingParamsCount) {
        for (int i = 0; i < o->smartComboRoutingParamsCount; i++) {
            sk_free(o->smartComboRoutingParams[i].tag);
            sk_free(o->smartComboRoutingParams[i].val);
        }

        sk_free(o->smartComboRoutingParams);
    }

    if (o->orderMiscOptions && o->orderMiscOptionsCount) {
        for (int i = 0; i < o->orderMiscOptionsCount; i++) {
            sk_free(o->orderMiscOptions[i].tag);
            sk_free(o->orderMiscOptions[i].val);
        }

        sk_free(o->orderMiscOptions);
    }
}

void tws_init_order_state(tr_order_state_t *ost)
{
    memset(ost, 0, sizeof * ost);
}

void tws_destroy_order_state(tr_order_state_t *ost)
{
    sk_free(ost->warningText);
    sk_free(ost->commissionCurrency);
    sk_free(ost->equityWithLoan);
    sk_free(ost->maintMargin);
    sk_free(ost->initMargin);
    sk_free(ost->status);
}

void tws_init_contract_details(tr_contract_details_t *cd)
{
    memset(cd, 0, sizeof * cd);
    tws_init_contract(&cd->summary);
}

void tws_destroy_contract_details(tr_contract_details_t *cd)
{
    sk_free(cd->marketName);
    sk_free(cd->orderTypes);
    sk_free(cd->validExchanges);
    sk_free(cd->longName);
    sk_free(cd->contractMonth);
    sk_free(cd->industry);
    sk_free(cd->category);
    sk_free(cd->subcategory);
    sk_free(cd->timeZoneId);
    sk_free(cd->tradingHours);
    sk_free(cd->liquidHours);
    sk_free(cd->evRule);
    if (cd->secIdList && cd->secIdListCount) {
        for (int i = 0; i < cd->secIdListCount; i++) {
            sk_free(cd->secIdList[i].tag);
            sk_free(cd->secIdList[i].val);
        }

        sk_free(cd->secIdList);
    }

    sk_free(cd->cusip);
    sk_free(cd->ratings);
    sk_free(cd->descAppend);
    sk_free(cd->bondType);
    sk_free(cd->couponType);
    sk_free(cd->maturity);
    sk_free(cd->issueDate);
    sk_free(cd->nextOptionType);
    sk_free(cd->nextOptionDate);
    sk_free(cd->notes);
    tws_destroy_contract(&cd->summary);
}

void tws_init_execution(tr_execution_t *exec)
{
    memset(exec, 0, sizeof * exec);
}

void tws_destroy_execution(tr_execution_t *exec)
{
    sk_free(exec->execId);
    sk_free(exec->time);
    sk_free(exec->acctNumber);
    sk_free(exec->exchange);
    sk_free(exec->side);
    sk_free(exec->orderRef);
    sk_free(exec->evRule);
}

void tws_init_tr_comboleg(tr_comboleg_t *cl)
{
    memset(cl, 0, sizeof(*cl));
    cl->exemptCode = -1;
}

void tws_destroy_tr_comboleg(tr_comboleg_t *cl)
{
    sk_free(cl->action);
    sk_free(cl->exchange);
    sk_free(cl->designatedLocation);
}

void tws_init_exec_filter(tr_exec_filter_t *filter)
{
    memset(filter, 0, sizeof(*filter));
}

void tws_destroy_exec_filter(tr_exec_filter_t *filter)
{
    sk_free(filter->acctCode);
    sk_free(filter->time);
    sk_free(filter->symbol);
    sk_free(filter->secType);
    sk_free(filter->exchange);
    sk_free(filter->side);
}

void tws_init_scanner_subscription(tr_scanner_subscription_t *ss)
{
    memset(ss, 0, sizeof(*ss));

    ss->abovePrice = DBL_MAX;
    ss->belowPrice = DBL_MAX;
    ss->aboveVolume = INT_MAX;
    ss->averageOptionVolumeAbove = INT_MAX;
    ss->marketCapAbove = DBL_MAX;
    ss->marketCapBelow = DBL_MAX;
    ss->couponRateAbove = DBL_MAX;
    ss->couponRateBelow = DBL_MAX;
    ss->numberOfRows = -1;
}

void tws_destroy_scanner_subscription(tr_scanner_subscription_t *ss)
{
    sk_free(ss->instrument);
    sk_free(ss->locationCode);
    sk_free(ss->scanCode);
    sk_free(ss->moodyRatingAbove);
    sk_free(ss->moodyRatingBelow);
    sk_free(ss->spRatingAbove);
    sk_free(ss->spRatingBelow);
    sk_free(ss->maturityDateAbove);
    sk_free(ss->maturityDateBelow);
    sk_free(ss->excludeConvertible);
    sk_free(ss->scannerSettingPairs);
    sk_free(ss->stockTypeFilter);
}

void tws_init_tag_value(tr_tag_value_t *t)
{
    memset(t, 0, sizeof(*t));
}

void tws_destroy_tag_value(tr_tag_value_t *t)
{
    sk_free(t->tag);
    sk_free(t->val);
}

void tws_init_order_combo_leg(tr_order_combo_leg_t *ocl)
{
    memset(ocl, 0, sizeof(*ocl));
    ocl->price = DBL_MAX;
}

void tws_destroy_order_combo_leg(tr_order_combo_leg_t *ocl)
{
}

void tws_init_under_comp(under_comp_t *u)
{
    memset(u, 0, sizeof(*u));
}

void tws_destroy_under_comp(under_comp_t *u)
{
}

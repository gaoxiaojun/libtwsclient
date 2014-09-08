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
    free(c->symbol);
    free(c->secType);
    free(c->expiry);
    free(c->right);
    free(c->multiplier);
    free(c->exchange);
    free(c->currency);
    free(c->localSymbol);
    free(c->tradingClass);
    free(c->primaryExch);
    free(c->secIdType);
    free(c->secId);
    free(c->comboLegsDescrip);

    if (c->comboLegs && c->comboLegsCount) {
        for (int i = 0; i < c->comboLegsCount; i++) {
            free(c->comboLegs[i].action);
            free(c->comboLegs[i].exchange);
            free(c->comboLegs[i].designatedLocation);
        }
        free(c->comboLegs);
    }

    free(c->underComp);
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
    free(o->action);
    free(o->orderType);
    free(o->tif);
    free(o->activeStartTime);
    free(o->activeStopTime);
    free(o->ocaGroup);
    free(o->orderRef);
    free(o->goodAfterTime);
    free(o->goodTillDate);
    free(o->rule80A);
    free(o->faGroup);
    free(o->faProfile);
    free(o->faPercentage);
    free(o->faMethod);
    free(o->openClose);
    free(o->designatedLocation);

    free(o->deltaNeutralOrderType);
    free(o->deltaNeutralSettlingFirm);
    free(o->deltaNeutralClearingAccount);
    free(o->deltaNeutralClearingIntent);
    free(o->deltaNeutralOpenClose);
    free(o->deltaNeutralDesignatedLocation);
    free(o->scaleTable);
    free(o->hedgeType);
    free(o->hedgeParam);
    free(o->account);
    free(o->settlingFirm);
    free(o->clearingAccount);
    free(o->clearingIntent);
    free(o->algoStrategy);

    if (o->algoParams && o->algoParamsCount) {
        for (int i = 0; i < o->algoParamsCount; i++) {
            free(o->algoParams[i].tag);
            free(o->algoParams[i].val);
        }

        free(o->algoParams);
    }

    if (o->smartComboRoutingParams && o->smartComboRoutingParamsCount) {
        for (int i = 0; i < o->smartComboRoutingParamsCount; i++) {
            free(o->smartComboRoutingParams[i].tag);
            free(o->smartComboRoutingParams[i].val);
        }

        free(o->smartComboRoutingParams);
    }

    if (o->orderMiscOptions && o->orderMiscOptionsCount) {
        for (int i = 0; i < o->orderMiscOptionsCount; i++) {
            free(o->orderMiscOptions[i].tag);
            free(o->orderMiscOptions[i].val);
        }

        free(o->orderMiscOptions);
    }
}

void tws_init_order_state(tr_order_state_t *ost)
{
    memset(ost, 0, sizeof * ost);
}

void tws_destroy_order_state(tr_order_state_t *ost)
{
    free(ost->warningText);
    free(ost->commissionCurrency);
    free(ost->equityWithLoan);
    free(ost->maintMargin);
    free(ost->initMargin);
    free(ost->status);
}

void tws_init_contract_details(tr_contract_details_t *cd)
{
    memset(cd, 0, sizeof * cd);
    tws_init_contract(&cd->summary);
}

void tws_destroy_contract_details(tr_contract_details_t *cd)
{
    free(cd->marketName);
    free(cd->orderTypes);
    free(cd->validExchanges);
    free(cd->longName);
    free(cd->contractMonth);
    free(cd->industry);
    free(cd->category);
    free(cd->subcategory);
    free(cd->timeZoneId);
    free(cd->tradingHours);
    free(cd->liquidHours);
    free(cd->evRule);
    if (cd->secIdList && cd->secIdListCount) {
        for (int i = 0; i < cd->secIdListCount; i++) {
            free(cd->secIdList[i].tag);
            free(cd->secIdList[i].val);
        }

        free(cd->secIdList);
    }

    free(cd->cusip);
    free(cd->ratings);
    free(cd->descAppend);
    free(cd->bondType);
    free(cd->couponType);
    free(cd->maturity);
    free(cd->issueDate);
    free(cd->nextOptionType);
    free(cd->nextOptionDate);
    free(cd->notes);
    tws_destroy_contract(&cd->summary);
}

void tws_init_execution(tr_execution_t *exec)
{
    memset(exec, 0, sizeof * exec);
}

void tws_destroy_execution(tr_execution_t *exec)
{
    free(exec->execId);
    free(exec->time);
    free(exec->acctNumber);
    free(exec->exchange);
    free(exec->side);
    free(exec->orderRef);
    free(exec->evRule);
}

void tws_init_tr_comboleg(tr_comboleg_t *cl)
{
    memset(cl, 0, sizeof(*cl));
    cl->exemptCode = -1;
}

void tws_destroy_tr_comboleg(tr_comboleg_t *cl)
{
    free(cl->action);
    free(cl->exchange);
    free(cl->designatedLocation);
}

void tws_init_exec_filter(tr_exec_filter_t *filter)
{
    memset(filter, 0, sizeof(*filter));
}

void tws_destroy_exec_filter(tr_exec_filter_t *filter)
{
    free(filter->acctCode);
    free(filter->time);
    free(filter->symbol);
    free(filter->secType);
    free(filter->exchange);
    free(filter->side);
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
    free(ss->instrument);
    free(ss->locationCode);
    free(ss->scanCode);
    free(ss->moodyRatingAbove);
    free(ss->moodyRatingBelow);
    free(ss->spRatingAbove);
    free(ss->spRatingBelow);
    free(ss->maturityDateAbove);
    free(ss->maturityDateBelow);
    free(ss->excludeConvertible);
    free(ss->scannerSettingPairs);
    free(ss->stockTypeFilter);
}

void tws_init_tag_value(tr_tag_value_t *t)
{
    memset(t, 0, sizeof(*t));
}

void tws_destroy_tag_value(tr_tag_value_t *t)
{
    free(t->tag);
    free(t->val);
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

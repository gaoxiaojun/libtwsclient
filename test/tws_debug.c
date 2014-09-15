#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <stdarg.h>
#include <string.h>
#include <inttypes.h>
#include <twsclient/tws_client.h>

/* utility */
const char *tws_tick_type_names[] = {
    "bidSize",
    "bidPrice",
    "askPrice",
    "askSize",
    "lastPrice",
    "lastSize",
    "high",
    "low",
    "volume",
    "close",
    "bidOptComp",
    "askOptComp",
    "lastOptComp",
    "modelOptComp",
    "open",
    "13WeekLow",
    "13WeekHigh",
    "26WeekLow",
    "26WeekHigh",
    "52WeekLow",
    "52WeekHigh",
    "AvgVolume",
    "OpenInterest",
    "OptionHistoricalVolatility",
    "OptionImpliedVolatility",
    "OptionBidExchStr",
    "OptionAskExchStr",
    "OptionCallOpenInterest",
    "OptionPutOpenInterest",
    "OptionCallVolume",
    "OptionPutVolume",
    "IndexFuturePremium",
    "bidExch",
    "askExch",
    "auctionVolume",
    "auctionPrice",
    "auctionImbalance",
    "markPrice",
    "bidEFP",
    "askEFP",
    "lastEFP",
    "openEFP",
    "highEFP",
    "lowEFP",
    "closeEFP",
    "lastTimestamp",
    "shortable",
    "fundamentals",
    "RTVolume",
    "halted",
    "bidYield",
    "askYield",
    "lastYield",
    "custOptComp",
    "trades",
    "trades/min",
    "volume/min",
    "lastRTHTrade",
    "RTHistoricalVolatility"
};

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

const char *fa_msg_name[] = { "(unknown)", "GROUPS", "PROFILES", "ALIASES" };
const char *tws_market_data_type_name[] = { "(unknown)", "Real Time", "Frozen" };
const char *fa_msg_type_name(tr_fa_msg_type_t x)
{
    unsigned int idx = (int)x;

    if (idx < ARRAY_SIZE(fa_msg_name)) {
        return fa_msg_name[idx];
    }

    return "(unknown)";
}

const char *tick_type_name(tr_tick_type_t x)
{
    unsigned int idx = (int)x;

    if (idx < ARRAY_SIZE(tws_tick_type_names)) {
        return tws_tick_type_names[idx];
    }

    return "(unknown)";
}

const char *market_data_type_name(market_data_type_t x)
{
    unsigned int idx = (int)x;

    if (idx < ARRAY_SIZE(tws_market_data_type_name)) {
        return tws_market_data_type_name[idx];
    }

    return "(unknown)";
}

const char *tr_comboleg_type_name(tr_comboleg_type_t x)
{
    unsigned int idx = (int)x;
    static const char *names[] = {
        "SAME", "OPEN", "CLOSE", "(unknown)"
    };

    if (idx < ARRAY_SIZE(names)) {
        return names[idx];
    }

    return "(unknown)";
}

const char *tr_origin_name(tr_origin_t x)
{
    unsigned int idx = (int)x;
    static const char *names[] = {
        "CUSTOMER", "FIRM"
    };

    if (idx < ARRAY_SIZE(names)) {
        return names[idx];
    }

    return "(unknown)";
}

const char *tr_oca_type_name(tr_oca_type_t x)
{
    unsigned int idx = (int)x;
    static const char *names[] = {
        "UNDEFINED", "CANCEL_WITH_BLOCK", "REDUCE_WITH_BLOCK", "REDUCE_NON_BLOCK"
    };

    if (idx < ARRAY_SIZE(names)) {
        return names[idx];
    }

    return "(unknown)";
}

const char *tr_auction_strategy_name(tr_auction_strategy_t x)
{
    unsigned int idx = (int)x;
    static const char *names[] = {
        "UNDEFINED", "MATCH", "IMPROVEMENT", "TRANSPARENT"
    };

    if (idx < ARRAY_SIZE(names)) {
        return names[idx];
    }

    return "(unknown)";
}

extern void tws_cb_printf(int indent_level, const char *msg, ...)
#ifdef __GNUC__
__attribute__((format(printf, 2, 3)))
#endif
;

void tws_cb_printf(int indent_level, const char *msg, ...)
{
    for (int i = 0; i < indent_level; i++)
        printf("\t");

    va_list ap;
    va_start(ap, msg);
    vprintf(msg, ap);
    va_end(ap);
}

static void tws_cb_print_under_comp(int indent_level, const under_comp_t *und)
{
    tws_cb_printf(indent_level, "UNDER_COMP: price=%g, delta=%g, conId=%d\n",
                  und->price, und->delta, und->conId);
}

static void tws_cb_print_tag_value_set(int indent_level, int count, const tr_tag_value_t *set)
{
    int i;

    tws_cb_printf(indent_level, "TAG/VALUE SET: count=%d\n", count);

    for (i = 0; i < count; i++) {
        const tr_tag_value_t *t = &set[i];

        tws_cb_printf(indent_level + 1, "TAG/VALUE [%d]: tag=[%s], val=[%s]\n",
                      i, t->tag, t->val);
    }
}

static void tws_cb_print_order_combolegs(int indent_level, int count, const tr_order_combo_leg_t *legs)
{
    int i;

    tws_cb_printf(indent_level, "ORDER COMBOLEGS: count=%d\n", count);

    for (i = 0; i < count; i++) {
        const tr_order_combo_leg_t *leg = &legs[i];

        tws_cb_printf(indent_level + 1, "ORDER COMBOLEG[%d]: price=%g\n",
                      i, leg->price);
    }
}

static void tws_cb_print_combolegs(int indent_level, int count, const tr_comboleg_t *legs)
{
    int i;
    (void)legs;
    tws_cb_printf(indent_level, "COMBOLEGS: count=%d\n", count);

    for (i = 0; i < count; i++) {
        const tr_comboleg_t *leg = &legs[i];

        tws_cb_printf(indent_level + 1, "COMBOLEG[%d]: action=[%s], exchange=[%s], designated_location=[%s], conId=%d, ratio=%d, open_close=%d (%s), short_sale_slot=%d, exempt_code=%d\n",
                      i, leg->action, leg->exchange, leg->designatedLocation, leg->conId, leg->ratio, leg->openClose,
                      tr_comboleg_type_name(leg->openClose), leg->shortSaleSlot, leg->exemptCode);
    }
}

static void tws_cb_print_contract(int indent_level, const tr_contract_t *contract)
{
    tws_cb_printf(indent_level, "CONTRACT: delta neutral:\n");

    /*if (contract->underComp) {
        tws_cb_print_under_comp(indent_level + 1, contract->underComp);
    }*/

    tws_cb_printf(indent_level + 1, "strike=%g, symbol=[%s], sectype=[%s], exchange=[%s], primaryExch=[%s], expiry=[%s], currency=[%s], right=[%s], localSymbol=[%s], multiplier=[%s], combolegsDescrip=[%s], secIdType=[%s], secId=[%s], conId=%d, includeExpired=%d\n",
                  contract->strike, contract->symbol, contract->secType, contract->exchange, contract->primaryExch, contract->expiry,
                  contract->currency, contract->right, contract->localSymbol, contract->multiplier,
                  contract->comboLegsDescrip, contract->secIdType, contract->secId, contract->conId, contract->includeExpired);
    tws_cb_print_combolegs(2, contract->comboLegsCount, contract->comboLegs);
}

static void tws_cb_print_contract_details(const tr_contract_details_t *cd)
{
    tws_cb_printf(1, "CONTRACT_DETAILS: contract summary:\n");
    tws_cb_print_contract(2, &cd->summary);
    tws_cb_printf(2, "min.tick: %g, coupon: %g, order types: [%s], valid exch: [%s], cusip: [%s], maturity: [%s], issue_date: [%s], ratings: [%s], bond_type: [%s], "
                  "coupon_type: [%s], notes: [%s], long name: [%s], industry: [%s], category: [%s], subcategory: [%s], timezone: [%s], trading hours: [%s], liquid hours: [%s], price_magnifier: %d, "
                  "under_conId: %d, ev_multiplier: %g, ev_rule: [%s]\n",
                  cd->minTick, cd->coupon, cd->orderTypes, cd->validExchanges, cd->cusip, cd->maturity, cd->issueDate, cd->ratings, cd->bondType,
                  cd->couponType, cd->notes, cd->longName, cd->industry, cd->category, cd->subcategory, cd->timeZoneId, cd->tradingHours, cd->liquidHours,
                  cd->priceMagnifier, cd->underConId, cd->evMultiplier, cd->evRule);
    tws_cb_print_tag_value_set(2, cd->secIdListCount, cd->secIdList);
}

static void tws_cb_print_order(const tr_order_t *order)
{
    tws_cb_printf(1, "ORDER: discretionary_amt=%g, lmt_price=%g, aux_price=%g, percent_offset=%g, nbbo_price_cap=%g, starting_price=%g, stock_ref_price=%g, delta=%g, stock_range_lower=%g, stock_range_upper=%g, volatility=%g, delta_neutral_aux_price=%g, trail_stop_price=%g, trailing_percent=%g, basis_points=%g, scale_price_increment=%g, scale_price_adjust_value=%g, scale_profit_offset=%g order_ref=%s\n",
                  order->discretionaryAmt, order->lmtPrice, order->auxPrice, order->percentOffset,
                  order->nbboPriceCap, order->startingPrice, order->stockRefPrice, order->delta,
                  order->stockRangeLower, order->stockRangeUpper, order->volatility, order->deltaNeutralAuxPrice,
                  order->trailStopPrice, order->trailingPercent, order->basisPoints, order->scalePriceIncrement,
                  order->scalePriceAdjustValue, order->scaleProfitOffset, order->orderRef);

    tws_cb_printf(2, "algo_strategy=[%s], good_after_time=[%s], good_till_date=[%s], fagroup=[%s], famethod=[%s], fapercentage=[%s], faprofile=[%s], action=[%s], order_type=[%s], tif=[%s], oca_group=[%s], account=[%s], open_close=[%s], orderref=[%s], designated_location=[%s], rule80a=[%s], settling_firm=[%s], delta_neutral_order_type=[%s], clearing_account=[%s], clearing_intent=[%s], hedge_type=[%s], hedge_param=[%s], delta_neutral_settling_firm=[%s], delta_neutral_clearing_account=[%s], delta_neutral_clearing_intent=[%s]\n",
                  order->algoStrategy, order->goodAfterTime, order->goodTillDate, order->faGroup,
                  order->faMethod, order->faPercentage, order->faProfile, order->action,
                  order->orderType, order->tif, order->ocaGroup, order->account,
                  order->openClose, order->orderRef, order->designatedLocation, order->rule80A,
                  order->settlingFirm, order->deltaNeutralOrderType, order->clearingAccount, order->clearingIntent,
                  order->hedgeType, order->hedgeParam, order->deltaNeutralSettlingFirm, order->deltaNeutralClearingAccount,
                  order->deltaNeutralClearingIntent);

    tws_cb_printf(2, "algo_params:\n");
    tws_cb_print_tag_value_set(3, order->algoParamsCount, order->algoParams);
    tws_cb_printf(2, "Smart combo routing params:\n");
    tws_cb_print_tag_value_set(3, order->smartComboRoutingParamsCount, order->smartComboRoutingParams);

    tws_cb_print_order_combolegs(2, order->orderComboLegsCount, order->orderComboLegs);

    tws_cb_printf(2, "o_orderid=%d, total_quantity=%d, origin=%d (%s), clientid=%d, permid=%d, parentid=%d, display_size=%d, trigger_method=%d, min_qty=%d, volatility_type=%d, reference_price_type=%d, basis_points_type=%d, scale_subs_level_size=%d, scale_init_level_size=%d, scale_price_adjust_interval=%d, scale_init_position=%d, scale_init_fill_qty=%d, exempt_code=%d, delta_neutral_con_id=%d, oca_type=%d (%s), auction_strategy=%d (%s)"
                  ", short_sale_slot=%d, override_percentage_constraints=%d, firm_quote_only=%d, etrade_only=%d, all_or_none=%d, outside_rth=%d, hidden=%d, transmit=%d, block_order=%d, sweep_to_fill=%d, continuous_update=%d, whatif=%d, not_held=%d, opt_out_smart_routing=%d, scale_auto_reset=%d, scale_random_percent=%d\n",
                  order->orderId, order->totalQuantity, order->origin, tr_origin_name(order->origin),
                  order->clientId, order->permId, order->parentId, order->displaySize,
                  order->triggerMethod, order->minQty, order->volatilityType, order->referencePriceType,
                  order->basisPointsType, order->scaleSubsLevelSize, order->scaleInitLevelSize, order->scalePriceAdjustInterval,
                  order->scaleInitPosition, order->scaleInitFillQty, order->exemptCode, order->deltaNeutralConId,
                  order->ocaType, tr_oca_type_name(order->ocaType), order->auctionStrategy, tr_auction_strategy_name(order->auctionStrategy),
                  order->shortSaleSlot, order->overridePercentageConstraints, order->firmQuoteOnly, order->eTradeOnly,
                  order->allOrNone, order->outsideRth, order->hidden, order->transmit,
                  order->blockOrder, order->sweepToFill, order->continuousUpdate, order->whatIf,
                  order->notHeld, order->optOutSmartRouting, order->scaleAutoReset, order->scaleRandomPercent);
}

static void tws_cb_print_order_status(const tr_order_state_t *ostatus)
{
    tws_cb_printf(1, "ORDER_STATUS: ost_commission=%g, ost_min_commission=%g, ost_max_commission=%g, ost_status=[%s], ost_init_margin=[%s], ost_maint_margin=[%s], ost_equity_with_loan=[%s], ost_commission_currency=[%s], ost_warning_text=[%s]\n",
                  ostatus->commission, ostatus->minCommission, ostatus->maxCommission, ostatus->status,
                  ostatus->initMargin, ostatus->maintMargin, ostatus->equityWithLoan, ostatus->commissionCurrency, ostatus->warningText);
}

static void tws_cb_print_execution(const tr_execution_t *exec)
{
    tws_cb_printf(1, "EXECUTION: e_price=%g, e_avg_price=%g, e_execid=[%s], e_time=[%s], e_acct_number=[%s], e_exchange=[%s], e_side=[%s], e_orderref=[%s], e_shares=%d, e_permid=%d, e_clientid=%d, e_liquidation=%d, e_orderid=%d, e_cum_qty=%d\n",
                  exec->price, exec->avgPrice, exec->execId, exec->time, exec->acctNumber, exec->exchange, exec->side, exec->orderRef, exec->shares,
                  exec->permId, exec->clientId, exec->liquidation, exec->orderId, exec->cumQty);
}

static void tws_cb_print_commission_report(tr_commission_report_t *report)
{
    tws_cb_printf(1, "COMMISSION_REPORT: cr_exec_id=[%s], cr_currency=[%s], cr_commission=%g, cr_realized_pnl=%g, cr_yield=%g, cr_yield_redemption_date=%d (%08X) (YYYYMMDD format)\n",
                  report->execId, report->currency, report->commission, report->realizedPNL, report->yield,
                  report->yieldRedemptionDate, report->yieldRedemptionDate);
}



/* event process */
void on_client_connected(tws_client_t *client)
{
    tr_contract_t contract;
    tws_init_contract(&contract);

    contract.currency = strdup("USD");
    contract.symbol = strdup("EUR");
    //contract.localSymbol = strdup("EUR.USD");
    contract.secType = strdup("CASH");
    contract.exchange = strdup("IDEALPRO");
    //contract.conId = 12087792;
    //tws_client_req_managed_accts(client);
    //tws_client_req_mkt_data(client, 1, &contract, "165,232,233,293,294,295,318", 0, NULL, 0);
    //tws_client_req_contract_details(client, 200, &contract);
    //tws_client_req_fundamenta_data(client, 300, &contract, "ReportsFinSummary");
    //tws_client_req_account_updates(client, true, NULL);
    tws_client_req_all_open_orders(client);
    tws_client_req_current_time(client);
    //tws_client_req_news_bulletins(client, true);
    //tws_client_req_realtime_bars(client, 2, &contract, 5, "ASK", 1, NULL, 0);
    //tws_client_req_realtime_bars(client, 3, &contract, 5, "BID", 1, NULL, 0);
    //tws_client_req_mkt_depth(client, 4, &contract, 10, NULL, 0);
    tws_client_set_server_loglevel(client, 5);
    tws_client_req_positions(client);
    //tws_client_req_historical_data(client, 500, &contract, "20140911 9:00:00 CST", "3600 S", "1 min", "MIDPOINT", true, 1, NULL, 0);
    tws_destroy_contract(&contract);
}

void on_client_disconnected(tws_client_t *client)
{
    printf("tws closed!!\n");
    tws_client_stop(client);
}

void dbg_event_tick_price(event_tick_price_t *ud)
{
    printf("tickPrice:id=%d, type=%d(%s), price=%f, size=%d, canAuto=%d\n",
           ud->tickerId, ud->tickType, tick_type_name(ud->tickType), ud->price, ud->size, ud->canAutoExecute);
}

static void dbg_event_tick_size(event_tick_size_t *ud)
{
    tws_cb_printf(0, "tick_size: ticker_id=%d, type=%d (%s), size=%d\n",
                  ud->tickerId, (int)ud->tickType, tick_type_name(ud->tickType), ud->size);
}

static void dbg_event_tick_option_computation(event_tick_option_computation_t *ud)
{
    tws_cb_printf(0, "tick option computation: ticker_id=%d, type=%d (%s), implied_vol=%g, delta=%g, opt_price=%g, pv_dividend=%g, gamma=%g, vega=%g, theta=%g, und_price=%g\n",
                  ud->tickerId, (int)ud->tickType, tick_type_name(ud->tickType), ud->impliedVol,
                  ud->delta, ud->optPrice, ud->pvDividend, ud->gamma,
                  ud->vega, ud->theta, ud->undPrice);
}

static void dbg_event_tick_generic(event_tick_generic_t *ud)
{
    tws_cb_printf(0, "tick_generic: ticker_id=%d, type=%d (%s), value=%g\n", ud->tickerId, ud->tickType,
                  tick_type_name(ud->tickType), ud->value);
}

static void dbg_event_tick_string(event_tick_string_t *ud)
{
    tws_cb_printf(0, "tick_string: ticker_id=%d, type=%d (%s), value=[%s]\n", ud->tickerId,
                  ud->tickType, tick_type_name(ud->tickType), ud->value);
}

static void dbg_event_tick_efp(event_tick_efp_t *ud)
{
    tws_cb_printf(0, "tick_efp: ticker_id=%d, type=%d (%s), basis_points=%g, formatted_basis_points=[%s], implied_futures_price=%g, hold_days=%d, future_expiry=[%s], dividend_impact=%g, dividends_to_expiry=%g\n",
                  ud->tickerId, ud->tickType, tick_type_name(ud->tickType), ud->basisPoints, ud->formatteBasisPoints,
                  ud->impliedFuturesPrice, ud->holdDays, ud->futureExpiry, ud->dividenImpact, ud->dividensToExpiry);
}

static void dbg_event_order_status(event_order_status_t *ud)
{
    tws_cb_printf(0, "order_status: order_id=%d, status=[%s], filled=%d, remaining %d, avg_fill_price=%g, last_fill_price=%g, perm_id=%d, parent_id=%d, client_id=%d, why_held=[%s]\n",
                  ud->id, ud->status, ud->filled, ud->remaining, ud->avgFillPrice,
                  ud->lastFillPrice, ud->permId, ud->parentId, ud->clientId, ud->whyHeld);
}

static void dbg_event_open_order(event_open_order_t *ud)
{
    /* commission values might be DBL_MAX */
    if (fabs(ud->orderState->commission - DBL_MAX) < DBL_EPSILON)
        tws_cb_printf(0, "open_order: commission not reported\n");
    else
        tws_cb_printf(0, "open_order: commission for order_id=%d was %.4g\n", ud->order->orderId, ud->orderState->commission);

    tws_cb_printf(0, "open_order: order_id=%d, sym=[%s], local_symbol=[%s]\n", ud->order->orderId,
                  ud->contract->symbol, ud->contract->localSymbol);
    tws_cb_print_contract(1, ud->contract);
    tws_cb_print_order(ud->order);
    tws_cb_print_order_status(ud->orderState);
}

static void dbg_event_update_account_value(event_acct_value_t *ud)
{
    tws_cb_printf(0, "update_account_value:key=[%s] val=[%s], currency=[%s], name=[%s]\n",
                  ud->key, ud->val, ud->currency, ud->accoutName);
}

static void dbg_event_update_portfolio(event_portfolio_value_t *ud)
{
    tws_cb_printf(0, "update_portfolio:sym=%s, position=%d, mkt_price=%.4g, mkt_value=%.4g, avg_cost=%.4g, unrealized_pnl=%.4g, realized_pnl=%.4g account_name=%s\n",
                  ud->contract->symbol, ud->position, ud->marketPrice, ud->marketValue,
                  ud->averageCost, ud->unrealizedPNL, ud->realizedPNL, ud->accountName);
    tws_cb_print_contract(1, ud->contract);
}

static void dbg_event_update_account_time(event_acct_update_time_t *ud)
{
    tws_cb_printf(0, "update_account_time: time_stamp=[%s]\n", ud->timeStamp);
}

static void dbg_event_next_valid_id(tws_client_t *client, event_next_valid_id_t *ud)
{
    int id = ud->orderId;
    tws_cb_printf(0, "next_valid_id for order placement %d\n", ud->orderId);
    tr_contract_t contract;
    tws_init_contract(&contract);

    contract.symbol = strdup("EUR");
    contract.secType = strdup("CASH");
    contract.exchange = strdup("IDEALPRO");
    contract.currency = strdup("USD");

    tr_order_t order;
    tws_init_order(&order);

    order.action = strdup("BUY");
    order.totalQuantity = 50000;
    order.orderType = strdup("MKT");

    tws_client_place_order(client, id, &contract, &order);
    tws_destroy_contract(&contract);
    tws_destroy_order(&order);
    tws_cb_printf(0, "send order\n");
    //tws_client_req_executions(client, 10, NULL);
}

static void dbg_event_contract_details(event_contract_data_t *ud)
{
    tws_cb_printf(0, "contract_details: req_id=%d, ...\n", ud->reqId);
    tws_cb_print_contract_details(ud->contractDetails);
}

static void dbg_event_bond_contract_details(event_bond_contract_data_t *ud)
{
    tws_cb_printf(0, "bond_contract_details: req_id=%d, ...\n", ud->reqId);
    tws_cb_print_contract_details(ud->contractDetails);
}

static void dbg_event_exec_details(event_execution_data_t *ud)
{
    tws_cb_printf(0, "exec_details: req_id=%d, ...\n", ud->reqId);
    tws_cb_print_contract(1, ud->contract);
    tws_cb_print_execution(ud->exec);
}

static void dbg_event_error(event_err_msg_t *ud)
{
    tws_cb_printf(0, "error: ticker_id=%d, error_code=%d, msg='%s'\n", ud->id, ud->errorCode, ud->errorMsg);
}

static void dbg_event_update_mkt_depth(event_market_depth_t *ud)
{
    tws_cb_printf(0, "update_mkt_depth: ticker_id=%d, posaition=%d, operation=%d, size=%d, price=%g, size=%d\n",
                  ud->id, ud->position, ud->operation, ud->side, ud->price, ud->size);
}

static void dbg_event_update_mkt_depth_l2(event_market_depth_l2_t *ud)
{
    tws_cb_printf(0, "update_mkt_depth_l2: ticker_id=%d, position=%d, market_maker=[%s], operation=%d, side=%d, price=%g, size=%d\n",
                  ud->id, ud->position, ud->marketMaker, ud->operation, ud->side, ud->price, ud->side);
}

static void dbg_event_update_news_bulletin(event_news_bulletins_t *ud)
{
    tws_cb_printf(0, "update_news_bulletin: msg_id=%d, msg_type=%d, msg=[%s], origin_exchange=[%s]\n",
                  ud->newsMsgId, ud->newsMsgType, ud->newsMessage, ud->originationgExch);
}

static void dbg_event_managed_accounts(event_managed_accts_t *ud)
{
    tws_cb_printf(0, "managed_accounts: accounts_list=[%s]\n",
                  ud->acctList);
}

static void dbg_event_receive_fa(event_receive_fa_t *ud)
{
    tws_cb_printf(0, "receive_fa: fa_data_type=%d (%s), xml='%s'\n", (int)ud->faDataType, fa_msg_type_name(ud->faDataType), ud->xml);
}

static void dbg_event_historical_data(event_historical_data_t *ud)
{
    tws_cb_printf(0, "historical: req_id=%d, start= %s end = %s count =%d\n", ud->reqId, ud->startDateStr, ud->endDateStr, ud->itemCount);
    for (int i = 0; i < ud->itemCount; i++) {
        tws_cb_printf(1, "barCount=%d, date=%s, ohlc=%.4g/%.4g/%.4g/%.4g volume=%d, wap=%.4g hasGaps=%d\n",
                      ud->items[i].barCount, ud->items[i].date, ud->items[i].open, ud->items[i].high, ud->items[i].low,
                      ud->items[i].close, ud->items[i].volume, ud->items[i].WAP, ud->items[i].hasGaps);
    }
    /*tws_cb_printf(0, "historical: req_id=%d, date=%s, ohlc=%.4g/%.4g/%.4g/%.4g, volume=%ld, bar_count=%d, wap=%.4g, has_gaps=%d\n",
                  req_id, date, open, high, low, close, volume, bar_count, wap, has_gaps);*/
}

static void dbg_event_scanner_parameters(event_scanner_parameters_t *ud)
{
    tws_cb_printf(0, "scanner_parameters: xml(len=%d)=[%s]\n", (int)strlen(ud->xml), ud->xml);
}

static void dbg_event_scanner_data(event_scanner_data_t *ud)
{
    (void)(ud);
    /*tws_cb_printf(0, "scanner_data: ticker_id=%d, rank=%d, distance=[%s], benchmark=[%s], projection=[%s], legs_str=[%s]\n",
                  ud->tickerId, rank, distance, benchmark, projection, legs_str);
    tws_cb_print_contract_details(cd);*/
}

static void dbg_event_current_time(event_current_time_t *ud)
{
    char tbuf[40];
    time_t timestamp = (time_t)ud->time;

    strftime(tbuf, sizeof(tbuf), "[%Y%m%dT%H%M%S] ", gmtime(&timestamp));

    tws_cb_printf(0, "current_time: time=%"PRId64" ~ '%s'\n", ud->time, tbuf);
}

static void dbg_event_realtime_bar(event_realtime_bars_t *ud)
{
    tws_cb_printf(0, "realtime_bar: req_id=%d, time=%"PRId64", ohlc=%.4g/%.4g/%.4g/%.4g, vol=%"PRId64", wap=%.4g, count=%d\n",
                  ud->reqId, ud->time, ud->open, ud->high, ud->low, ud->close, ud->volume, ud->wap, ud->count);
}

static void dbg_event_fundamental_data(event_fundamental_data_t *ud)
{
    tws_cb_printf(0, "fundamental_data: req_id=%d, data=[%s]\n", ud->reqId, ud->data);
}

static void dbg_event_contract_details_end(event_contract_data_end_t *ud)
{
    tws_cb_printf(0, "contract_details_end: req_id=%d\n", ud->reqId);
}

static void dbg_event_open_order_end(void *p)
{
    (void)(p);
    tws_cb_printf(0, "open_order_end\n");
}

static void dbg_event_delta_neutral_validation(event_delta_neutral_validation_t *ud)
{
    tws_cb_printf(0, "delta_neutral_validation: req_id=%d, ...\n", ud->reqId);
    tws_cb_print_under_comp(1, &ud->underComp);
}

static void dbg_event_acct_download_end(event_acct_download_end_t *ud)
{
    tws_cb_printf(0, "acct_download_end: account name=[%s]\n", ud->accountName);
}

static void dbg_event_exec_details_end(event_execution_data_end_t *ud)
{
    (void )ud;
    tws_cb_printf(0, "exec_details_end: req_id=%d\n", ud->reqId);
}

static void dbg_event_tick_snapshot_end(event_tick_snapshot_end_t *ud)
{
    tws_cb_printf(0, "tick_snapshot_end: req_id=%d\n", ud->reqId);
}

static void dbg_event_market_data_type(event_market_data_type_t *ud)
{
    tws_cb_printf(0, "market_data_type: req_id=%d, data_type=%d (%s)\n", ud->reqId,
                  (int)ud->marketDataType, market_data_type_name(ud->marketDataType));
}

static void dbg_event_commission_report(event_commission_report_t *ud)
{
    tws_cb_printf(0, "commission_report: ...reqId=%d\n", ud->reqId);
    tws_cb_print_commission_report(&ud->report);
}

static void dbg_event_position(event_position_t *ud)
{
    tws_cb_printf(0, "position_data: account=%s position=%d avg_cost=%f symbol=%s\n", ud->account,
                  ud->position, ud->avgCost, ud->contract->symbol);
    tws_cb_print_contract(0, ud->contract);
}

static void dbg_event_position_end(void *p)
{
    (void)(p);
    tws_cb_printf(0, "position_end\n");
}

static void dbg_event_account_summary(event_account_summary_t *ud)
{
    tws_cb_printf(0, "account_summary: req_id=%d, accout_name=%s, tag=%s, value=%s, currency=%s\n",
                  ud->reqId, ud->account, ud->tag, ud->value, ud->currency);
}

static void dbg_event_account_summary_end(event_account_summary_end_t *ud)
{
    tws_cb_printf(0, "account_summary_end: req_id=%d\n", ud->reqId);
}

static void dbg_event_verify_message_api(event_verify_message_api_t *ud)
{
    tws_cb_printf(0, "event_verify_message_api: api_data=%s\n", ud->apiData);
}

static void dbg_event_verify_completed(event_verify_completed_t *ud)
{
    tws_cb_printf(0, "event_verify_completed: isSuccessful=%s error_text=%s\n", ud->isSuccessful ? "True" : "False", ud->errorText);
}

static void dbg_event_display_group_list(event_display_group_list_t *ud)
{
    tws_cb_printf(0, "event_display_group_list: req_id=%d group=%s\n", ud->reqId, ud->groups);
}

static void dbg_event_display_group_update(event_display_group_updated_t *ud)
{
    tws_cb_printf(0, "event_display_group_update: req_id=%d contract_info=%s\n", ud->reqId, ud->contractInfo);
}



void tws_debug(tws_client_t *client, int event_type, void *ud)
{
    switch (event_type) {
    case CLIENT_CONNECTED:
        on_client_connected(client);
        break;
    case CLIENT_DISCONNECTED:
        on_client_disconnected(client);
        break;
    case TICK_PRICE:
        dbg_event_tick_price(ud);
        break;
    case TICK_SIZE:
        dbg_event_tick_size(ud);
        break;
    case POSITION:
        dbg_event_position(ud);
        break;
    case POSITION_END:
        dbg_event_position_end(ud);
        break;
    case ACCOUNT_SUMMARY:
        dbg_event_account_summary(ud);
        break;
    case ACCOUNT_SUMMARY_END:
        dbg_event_account_summary_end(ud);
        break;
    case TICK_OPTION_COMPUTATION:
        dbg_event_tick_option_computation(ud);
        break;
    case TICK_GENERIC:
        dbg_event_tick_generic(ud);
        break;
    case TICK_STRING:
        dbg_event_tick_string(ud);
        break;
    case TICK_EFP:
        dbg_event_tick_efp(ud);
        break;
    case ORDER_STATUS:
        dbg_event_order_status(ud);
        break;
    case ACCT_VALUE:
        dbg_event_update_account_value(ud);
        break;
    case PORTFOLIO_VALUE:
        dbg_event_update_portfolio(ud);
        break;
    case ACCT_UPDATE_TIME:
        dbg_event_update_account_time(ud);
        break;
    case ERR_MSG:
        dbg_event_error(ud);
        break;
    case OPEN_ORDER:
        dbg_event_open_order(ud);
        break;
    case NEXT_VALID_ID:
        dbg_event_next_valid_id(client, ud);
        break;
    case SCANNER_DATA:
        dbg_event_scanner_data(ud);
        break;
    case CONTRACT_DATA:
        dbg_event_contract_details(ud);
        break;
    case BOND_CONTRACT_DATA:
        dbg_event_bond_contract_details(ud);
        break;
    case EXECUTION_DATA:
        dbg_event_exec_details(ud);
        break;
    case MARKET_DEPTH:
        dbg_event_update_mkt_depth(ud);
        break;
    case MARKET_DEPTH_L2:
        dbg_event_update_mkt_depth_l2(ud);
        break;
    case NEWS_BULLETINS:
        dbg_event_update_news_bulletin(ud);
        break;
    case MANAGED_ACCTS:
        dbg_event_managed_accounts(ud);
        break;
    case RECEIVE_FA:
        dbg_event_receive_fa(ud);
        break;
    case HISTORICAL_DATA:
        dbg_event_historical_data(ud);
        break;
    case SCANNER_PARAMETERS:
        dbg_event_scanner_parameters(ud);
        break;
    case CURRENT_TIME:
        dbg_event_current_time(ud);
        break;
    case REAL_TIME_BARS:
        dbg_event_realtime_bar(ud);
        break;
    case FUNDAMENTAL_DATA:
        dbg_event_fundamental_data(ud);
        break;
    case CONTRACT_DATA_END:
        dbg_event_contract_details_end(ud);
        break;
    case OPEN_ORDER_END:
        dbg_event_open_order_end(ud);
        break;
    case ACCT_DOWNLOAD_END:
        dbg_event_acct_download_end(ud);
        break;
    case EXECUTION_DATA_END:
        dbg_event_exec_details_end(ud);
        break;
    case DELTA_NEUTRAL_VALIDATION:
        dbg_event_delta_neutral_validation(ud);
        break;
    case TICK_SNAPSHOT_END:
        dbg_event_tick_snapshot_end(ud);
        break;
    case MARKET_DATA_TYPE:
        dbg_event_market_data_type(ud);
        break;
    case COMMISSION_REPORT:
        dbg_event_commission_report(ud);
        break;
    case VERIFY_MESSAGE_API:
        dbg_event_verify_message_api(ud);
        break;
    case VERIFY_COMPLETED:
        dbg_event_verify_completed(ud);
        break;
    case DISPLAY_GROUP_LIST:
        dbg_event_display_group_list(ud);
        break;
    case DISPLAY_GROUP_UPDATED:
        dbg_event_display_group_update(ud);
        break;
    default:
        printf("Unknow event type = %d\n", event_type);
    }
}

#ifndef __TWS_CLIENT_H__
#define __TWS_CLIENT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <uv.h>
#include <twsclient/tws_common.h>
#include <twsclient/tws_type.h>
#include <twsclient/tws_event.h>
#include <twsclient/tws_error.h>

/*
 *  The client require TWS version 9.71 or higher
 */
#define TWS_VERSION "9.71"

/* Public API */

/**
 * Create and initiate Tws client intance.
 *
 * @return Tws client instance
 */
EXPORT tws_client_t * tws_client_new(uv_loop_t *loop, tws_event_callback cb);

/**
 * Create and init tws client instance with reconnect enable
 *
 * @param delay delay time in second
 * @param delay_max the max delay time in second
 * @param exp_backoff whether enable exponetial backoff
 *
 * For example, if 2 -> delay, 10 -> delay_max, then the reconnect delay will be
 *   2, 4, 6, 8, 10, 10, 10 seconds...
 *   if 2 -> delay, 30 -> delay_max enable exponetial backoff, the reconnect delay will be
 *   2, 4, 8, 16, 30, 30 seconds...
 */
EXPORT tws_client_t* tws_client_new_with_reconnect(uv_loop_t *loop, tws_event_callback cb,
                                            int delay, int delay_max, int exp_backoff);

/**
 * Start the connection of the client.
 *
 * @param client client instance.
 */
EXPORT int tws_client_connect(tws_client_t *client, const char *host, unsigned int port);

/**
 * Close the connection of the client and waiting for reconnect if enable.
 *
 * @param client client instance.
 */
EXPORT void tws_client_close(tws_client_t *client);

/**
 * Stop the connection of the client and disable reconnect.
 *
 * @param client client instance.
 */
EXPORT void tws_client_stop(tws_client_t *client);

/**
 * Destroy and disconnect the connection of the client instance.
 *
 * @param client client instance.
 */
EXPORT void tws_client_destroy(tws_client_t *client);

/* Command API */
EXPORT void tws_client_cancel_scanner_subscription(tws_client_t *client, int tickerId);
EXPORT void tws_client_req_scanner_parameters(tws_client_t *client);
EXPORT void tws_client_req_scanner_subscription(tws_client_t *client, int tickerId,
                                         tr_scanner_subscription_t *subscription,
                                         tr_tag_value_t *options, int options_count);
EXPORT void tws_client_req_mkt_data(tws_client_t *client, int tickerId,
                             tr_contract_t *contract, char *generick_tick_list,
                             bool snapshot, tr_tag_value_t *options, int options_count);
EXPORT void tws_client_cancel_realtime_bars(tws_client_t *client, int tickerId);
EXPORT void tws_client_cancel_historical_data(tws_client_t *client, int tickerId);
EXPORT void tws_client_req_historical_data(tws_client_t *client, int tickerId,
                                    tr_contract_t *contract, char *endDataTime,
                                    char *durationStr, char *barSizeSetting,
                                    char *whatToShow, bool useRTH, int formatDate,
                                    tr_tag_value_t *options, int options_count);
EXPORT void tws_client_req_realtime_bars(tws_client_t *client, int tickerId,
                                  tr_contract_t *contract, int barSize,
                                  char *whatToShow, bool useRTH,
                                  tr_tag_value_t *options, int option_count);
EXPORT void tws_client_req_contract_details(tws_client_t *client, int reqId,
                                     tr_contract_t *contract);
EXPORT void tws_client_req_mkt_depth(tws_client_t *client, int tickerId,
                             tr_contract_t *contract, int numRows,
                             tr_tag_value_t *options, int options_count);
EXPORT void tws_client_cancel_mkt_data(tws_client_t *client, int tickerId);
EXPORT void tws_client_cancel_mkt_depth(tws_client_t *client, int tickerId);
EXPORT void tws_client_exercise_options(tws_client_t *client, int tickerId,
                                 tr_contract_t *contract, int exerciseAction,
                                 int exerciseQuantity, char *account, int override);

EXPORT void tws_client_req_account_updates(tws_client_t *client, bool subscribe, char *acctCode);
EXPORT void tws_client_req_executions(tws_client_t *client, int reqId, tr_exec_filter_t *filter);
EXPORT void tws_client_cancel_order(tws_client_t *client, int id);
EXPORT void tws_client_req_open_orders(tws_client_t *client);
EXPORT void tws_client_req_ids(tws_client_t *client, int numIds);
EXPORT void tws_client_req_news_bulletins(tws_client_t *client, bool allMsgs);
EXPORT void tws_client_cancel_news_bulletins(tws_client_t *client);
EXPORT void tws_client_set_server_loglevel(tws_client_t *client, int logLevel);
EXPORT void tws_client_req_auto_open_orders(tws_client_t *client, bool bAutoBind);
EXPORT void tws_client_req_all_open_orders(tws_client_t *client);
EXPORT void tws_client_req_managed_accts(tws_client_t *client);
EXPORT void tws_client_req_fa(tws_client_t *client, int faDataType);
EXPORT void tws_client_replace_fa(tws_client_t *client, int faDataType, char *xml);
EXPORT void tws_client_req_current_time(tws_client_t *client);
EXPORT void tws_client_req_fundamenta_data(tws_client_t *client, int reqId, tr_contract_t *contract,
                                    char *reportType);
EXPORT void tws_client_cancel_fundamenta_data(tws_client_t *client, int reqId);
EXPORT void tws_client_calculate_implied_volatility(tws_client_t *client, int reqId,
                                             tr_contract_t *contract,
                                             double optionPrice, double underPrice);
EXPORT void tws_client_cancel_calculate_implied_volatility(tws_client_t *client, int reqId);
EXPORT void tws_client_calculate_option_price(tws_client_t *client, int reqId,
                                             tr_contract_t *contract,
                                             double volatility, double underPrice);
EXPORT void tws_client_cancel_calculate_option_price(tws_client_t *client, int reqId);
EXPORT void tws_client_req_global_cancel(tws_client_t *client);
EXPORT void tws_client_req_market_data_type(tws_client_t *client, int marketDataType);
EXPORT void tws_client_req_positions(tws_client_t *client);
EXPORT void tws_client_cancel_positions(tws_client_t *client);
EXPORT void tws_client_req_account_summary(tws_client_t *client, int reqId, char *group, char *tags);
EXPORT void tws_client_cancel_account_summary(tws_client_t *client, int reqId);
EXPORT void tws_client_verify_request(tws_client_t *client, char *apiName, char *apiVersion);
EXPORT void tws_client_verify_message(tws_client_t *client, char *apiData);
EXPORT void tws_client_query_display_groups(tws_client_t *client, int reqId);
EXPORT void tws_client_subscribe_to_group_events(tws_client_t *client, int reqId, int groupId);
EXPORT void tws_client_unsubscribe_from_group_events(tws_client_t *client, int reqId);
EXPORT void tws_client_update_display_group(tws_client_t *client, int reqId, char *contractInfo);


#ifdef __cplusplus
}
#endif

#endif /* __TWS_CLIENT_H__ */

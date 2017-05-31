#ifndef SKYWALKER_TC_TWS_TWS_EVENT_H
#define SKYWALKER_TC_TWS_TWS_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <twsclient/tws_type.h>

/* event type */
#define TICK_PRICE                      1
#define TICK_SIZE                       2
#define ORDER_STATUS                    3
#define ERR_MSG                         4
#define OPEN_ORDER                      5
#define ACCT_VALUE                      6
#define PORTFOLIO_VALUE                 7
#define ACCT_UPDATE_TIME                8
#define NEXT_VALID_ID                   9
#define CONTRACT_DATA                   10
#define EXECUTION_DATA                  11
#define MARKET_DEPTH                    12
#define MARKET_DEPTH_L2                 13
#define NEWS_BULLETINS                  14
#define MANAGED_ACCTS                   15
#define RECEIVE_FA                      16
#define HISTORICAL_DATA                 17
#define BOND_CONTRACT_DATA              18
#define SCANNER_PARAMETERS              19
#define SCANNER_DATA                    20
#define TICK_OPTION_COMPUTATION         21
#define TICK_GENERIC                    45
#define TICK_STRING                     46
#define TICK_EFP                        47
#define CURRENT_TIME                    49
#define REAL_TIME_BARS                  50
#define FUNDAMENTAL_DATA                51
#define CONTRACT_DATA_END               52
#define OPEN_ORDER_END                  53
#define ACCT_DOWNLOAD_END               54
#define EXECUTION_DATA_END              55
#define DELTA_NEUTRAL_VALIDATION        56
#define TICK_SNAPSHOT_END               57
#define MARKET_DATA_TYPE                58
#define COMMISSION_REPORT               59
#define POSITION                        61
#define POSITION_END                    62
#define ACCOUNT_SUMMARY                 63
#define ACCOUNT_SUMMARY_END             64
#define VERIFY_MESSAGE_API              65
#define VERIFY_COMPLETED                66
#define DISPLAY_GROUP_LIST              67
#define DISPLAY_GROUP_UPDATED           68

#define CLIENT_CONNECTED                10000
#define CLIENT_DISCONNECTED             10001

/* event data struct */
typedef struct event_tick_price_t {
    int tickerId;
    int tickType;
    double price;
    int size;
    int sizeTickType;
    int canAutoExecute;
}event_tick_price_t;

typedef struct event_tick_size_t {
    int tickerId;
    int tickType;
    int size;
}event_tick_size_t;

typedef struct event_position_t {
    char *account;
    tr_contract_t *contract;
    int position;
    double avgCost;
}event_position_t;

typedef struct event_account_summary_t {
    int reqId;
    char *account;
    char *tag;
    char *value;
    char *currency;
}event_account_summary_t;

typedef struct event_account_summary_end_t {
    int reqId;
}event_account_summary_end_t;

typedef struct event_tick_option_computation_t {
    int tickerId;
    int tickType;
    double impliedVol;
    double delta;
    double optPrice;
    double pvDividend;
    double gamma;
    double vega;
    double theta;
    double undPrice;
}event_tick_option_computation_t;

typedef struct event_tick_generic_t {
    int tickerId;
    int tickType;
    double value;
}event_tick_generic_t;

typedef struct event_tick_string_t {
    int tickerId;
    int tickType;
    char *value;
}event_tick_string_t;

typedef struct event_tick_efp_t {
    int tickerId;
    int tickType;
    double basisPoints;
    char *formatteBasisPoints;
    double impliedFuturesPrice;
    int holdDays;
    char *futureExpiry;
    double dividenImpact;
    double dividensToExpiry;
}event_tick_efp_t;

typedef struct event_order_status_t {
    int id;
    char *status;
    int filled;
    int remaining;
    double avgFillPrice;
    int permId;
    int parentId;
    double lastFillPrice;
    int clientId;
    char *whyHeld;
}event_order_status_t;

typedef struct event_acct_value_t {
    char *key;
    char *val;
    char *currency;
    char *accoutName;
}event_acct_value_t;

typedef struct event_portfolio_value_t {
    tr_contract_t *contract;
    int position;
    double marketPrice;
    double marketValue;
    double averageCost;
    double unrealizedPNL;
    double realizedPNL;
    char *accountName;
}event_portfolio_value_t;

typedef struct event_acct_update_time_t {
    char *timeStamp;
}event_acct_update_time_t;

typedef struct event_err_msg_t {
    int id;
    int errorCode;
    char *errorMsg;
}event_err_msg_t;

typedef struct event_open_order_t {
    tr_contract_t *contract;
    tr_order_t *order;
    tr_order_state_t *orderState;
}event_open_order_t;

typedef struct event_next_valid_id_t {
    int orderId;
}event_next_valid_id_t;

typedef struct event_scanner_data_item_t {
    tr_contract_details_t *contractDetails;
    int rank;
    char *distance;
    char *benchmark;
    char *projection;
    char *legsStr;
}event_scanner_data_item_t;

typedef struct event_scanner_data_t {
    int tickerId;
    int numberOfElements;
    event_scanner_data_item_t *elements;
}event_scanner_data_t;

typedef struct event_contract_data_t {
    int reqId;
    tr_contract_details_t *contractDetails;
}event_contract_data_t;

typedef struct event_bond_contract_data_t {
    int reqId;
    tr_contract_details_t *contractDetails;
}event_bond_contract_data_t;

typedef struct event_execution_data_t {
    int reqId;
    tr_contract_t *contract;
    tr_execution_t *exec;
}event_execution_data_t;

typedef struct event_market_depth_t {
    int id;
    int position;
    int operation;
    int side;
    double price;
    int size;
}event_market_depth_t;

typedef struct event_market_depth_l2_t {
    int id;
    int position;
    char *marketMaker;
    int operation;
    int side;
    double price;
    int size;
}event_market_depth_l2_t;

typedef struct event_news_bulletins_t {
    int newsMsgId;
    int newsMsgType;
    char *newsMessage;
    char *originationgExch;
}event_news_bulletins_t;

typedef struct event_managed_accts_t {
    char *acctList;
}event_managed_accts_t;

typedef struct event_receive_fa_t {
    int faDataType;
    char *xml;
}event_receive_fa_t;

typedef struct event_historical_data_item_t {
    char *date;
    double open;
    double high;
    double low;
    double close;
    int volume;
    double WAP;
    int hasGaps;
    int barCount;
}event_historical_data_item_t;

typedef struct event_historical_data_t {
    int reqId;
    char *startDateStr;
    char *endDateStr;
    int itemCount;
    event_historical_data_item_t *items;
}event_historical_data_t;

typedef struct event_scanner_parameters_t {
    char *xml;
}event_scanner_parameters_t;

typedef struct event_current_time_t {
    int64_t time;
}event_current_time_t;

typedef struct event_realtime_bars_t {
    int reqId;
    int64_t time;
    double open;
    double high;
    double low;
    double close;
    int64_t volume;
    double wap;
    int count;
}event_realtime_bars_t;

typedef struct event_fundamental_data_t {
    int reqId;
    char *data;
}event_fundamental_data_t;

typedef struct event_contract_data_end_t {
    int reqId;
}event_contract_data_end_t;

typedef struct event_acct_download_end_t {
    char *accountName;
}event_acct_download_end_t;

typedef struct event_execution_data_end_t {
    int reqId;
}event_execution_data_end_t;

typedef struct event_delta_neutral_validation_t {
    int reqId;
    tr_under_comp_t underComp;
}event_delta_neutral_validation_t;

typedef struct event_tick_snapshot_end_t {
    int reqId;
}event_tick_snapshot_end_t;

typedef struct event_market_data_type_t {
    int reqId;
    int marketDataType;
}event_market_data_type_t;

typedef struct event_commission_report_t {
    tr_commission_report_t report;
}event_commission_report_t;

typedef struct event_verify_message_api_t {
    char *apiData;
}event_verify_message_api_t;

typedef struct event_verify_completed_t {
    int isSuccessful;
    char *errorText;
}event_verify_completed_t;

typedef struct event_display_group_list_t {
    int reqId;
    char *groups;
}event_display_group_list_t;

typedef struct event_display_group_updated_t {
    int reqId;
    char *contractInfo;
}event_display_group_updated_t;

#define EVENT_PRIVATE_FIELD \
    int version;    \
    int j

typedef struct tws_event_t {
    union {
        event_tick_price_t tick_price;
        event_tick_size_t tick_size;
        event_position_t position;
        event_account_summary_t account_summary;
        event_account_summary_end_t account_summary_end;
        event_tick_option_computation_t tick_option_computation;
        event_tick_generic_t tick_generic;
        event_tick_string_t tick_string;
        event_tick_efp_t tick_efp;
        event_order_status_t order_status;
        event_acct_value_t acct_value;
        event_portfolio_value_t portfolio_value;
        event_acct_update_time_t acct_update_time;
        event_err_msg_t err_msg;
        event_open_order_t open_order;
        event_next_valid_id_t next_valid_id;
        event_scanner_data_t scanner_data;
        event_contract_data_t contract_data;
        event_bond_contract_data_t bond_contract_data;
        event_execution_data_t execution_data;
        event_market_depth_t market_depth;
        event_market_depth_l2_t market_depth_l2;
        event_news_bulletins_t news_bulletins;
        event_managed_accts_t managed_accts;
        event_receive_fa_t receive_fa;
        event_historical_data_t historical_data;
        event_scanner_parameters_t scanner_parameters;
        event_current_time_t current_time;
        event_realtime_bars_t realtime_bars;
        event_fundamental_data_t fundamental_data;
        event_contract_data_end_t contract_data_end;
        event_acct_download_end_t acct_download_end;
        event_execution_data_end_t execution_data_end;
        event_delta_neutral_validation_t delta_neutral_validation;
        event_tick_snapshot_end_t tick_snapshot_end;
        event_market_data_type_t market_data_type;
        event_commission_report_t commission_report;
        event_verify_message_api_t verify_message_api;
        event_verify_completed_t verify_completed;
        event_display_group_list_t display_group_list;
        event_display_group_updated_t display_group_updated;
    }data;
    EVENT_PRIVATE_FIELD;
}tws_event_t;


/* Genrical Event Callback */
typedef void (*tws_event_callback)(tws_client_t *client, int event_type, void *event_data);

/* Callback API */
typedef void (*tws_event_connected_func_t)(tws_client_t *client);
typedef void (*tws_event_disconnected_func_t)(tws_client_t *client);

#ifdef __cplusplus
}
#endif

#endif  // SKYWALKER_TC_TWS_TWS_EVENT_H

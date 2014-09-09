#ifndef SKYWALKER_TC_TWS_TWS_TYPE_H
#define SKYWALKER_TC_TWS_TWS_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <twsclient/tws_common.h>

typedef enum {
  TWS_ST_CLOSED = 1,
  TWS_ST_DISCONNECTING,
  TWS_ST_CONNECTING,
  TWS_ST_CONNECTED,
  TWS_ST_AUTHED,
  TWS_ST_READY,   /* now the client ready for command */
} tws_client_state;

typedef void (*tws_logger)(const char *fmt, ...);

#define TWS_CLIENT_PUBLIC_FIELDS \
    void *data;                 \
    tws_client_state state;     \
    int serverVersion;          \
    int clientId;               \
    tws_logger logger;

typedef struct tws_client_s {
    TWS_CLIENT_PUBLIC_FIELDS
}tws_client_t;

typedef enum {
    FAMT_UNKNOWN  = 0,
    GROUPS        = 1,
    PROFILES      = 2,
    ALIASES       = 3,
} tr_fa_msg_type_t;

typedef enum {
    MDT_UNKNOWN = 0,
    REALTIME    = 1,
    FROZEN      = 2,
} market_data_type_t;

typedef enum {
    COMBOLEG_SAME    = 0,   /* open/close leg value is same as combo */
    COMBOLEG_OPEN    = 1,
    COMBOLEG_CLOSE   = 2,
    COMBOLEG_UNKNOWN = 3,
} tr_comboleg_type_t;


typedef enum tr_origin {
    CUSTOMER = 0,
    FIRM = 1
} tr_origin_t;

typedef enum tr_oca_type {
    OCA_UNDEFINED = 0,
    CANCEL_WITH_BLOCK = 1,
    REDUCE_WITH_BLOCK = 2,
    REDUCE_NON_BLOCK = 3
} tr_oca_type_t;

typedef enum tr_auction_strategy {
    AUCTION_UNDEFINED = 0,
    AUCTION_MATCH = 1,
    AUCTION_IMPROVEMENT = 2,
    AUCTION_TRANSPARENT = 3
} tr_auction_strategy_t;


typedef enum tr_tick_type {
    TICK_UNDEFINED = -1,
    BID_SIZE   = 0,
    BID        = 1,
    ASK        = 2,
    ASK_SIZE   = 3,
    LAST       = 4,
    LAST_SIZE  = 5,
    HIGH       = 6,
    LOW        = 7,
    VOLUME     = 8,
    CLOSE      = 9,
    BID_OPTION = 10,
    ASK_OPTION = 11,
    LAST_OPTION = 12,
    MODEL_OPTION = 13,
    OPEN         = 14,
    LOW_13_WEEK  = 15,
    HIGH_13_WEEK = 16,
    LOW_26_WEEK  = 17,
    HIGH_26_WEEK = 18,
    LOW_52_WEEK  = 19,
    HIGH_52_WEEK = 20,
    AVG_VOLUME   = 21,
    OPEN_INTEREST = 22,
    OPTION_HISTORICAL_VOL = 23, /* volatility */
    OPTION_IMPLIED_VOL = 24,    /* volatility */
    OPTION_BID_EXCH = 25,
    OPTION_ASK_EXCH = 26,
    OPTION_CALL_OPEN_INTEREST = 27,
    OPTION_PUT_OPEN_INTEREST = 28,
    OPTION_CALL_VOLUME = 29,
    OPTION_PUT_VOLUME = 30,
    INDEX_FUTURE_PREMIUM = 31,
    BID_EXCH = 32,
    ASK_EXCH = 33,
    AUCTION_VOLUME = 34,
    AUCTION_PRICE = 35,
    AUCTION_IMBALANCE = 36,
    MARK_PRICE = 37,
    BID_EFP_COMPUTATION  = 38,
    ASK_EFP_COMPUTATION  = 39,
    LAST_EFP_COMPUTATION = 40,
    OPEN_EFP_COMPUTATION = 41,
    HIGH_EFP_COMPUTATION = 42,
    LOW_EFP_COMPUTATION = 43,
    CLOSE_EFP_COMPUTATION = 44,
    LAST_TIMESTAMP = 45,
    SHORTABLE = 46,
    FUNDAMENTAL_RATIOS = 47,
    RT_VOLUME = 48,
    HALTED = 49,
    BID_YIELD = 50,
    ASK_YIELD = 51,
    LAST_YIELD = 52,
    CUST_OPTION_COMPUTATION = 53,
    TRADE_COUNT = 54,
    TRADE_RATE = 55,
    VOLUME_RATE = 56,
    LAST_RTH_TRADE = 57,
    REGULATORY_IMBALANCE = 61,
} tr_tick_type_t;

typedef struct _under_comp {
    int    conId;
    double delta;
    double price;
} under_comp_t;

typedef struct _comboleg {
    int   conId;
    int   ratio;
    char *action;                                    /* BUY/SELL/SSHORT/SSHORTX */
    char *exchange;
    tr_comboleg_type_t openClose;

    // for stock legs when doing short sale
    int   shortSaleSlot;                           /* 1 = clearing broker, 2 = third party */
    char     *designatedLocation;                       /* set to "" if unused, as usual */
    int   exemptCode;                               /* set to -1 if you do not use it */
} tr_comboleg_t;

typedef struct _contract {
    int            conId;
    char          *symbol;
    char          *secType;
    char          *expiry;
    double         strike;
    char          *right;
    char          *multiplier;
    char          *exchange;

    char          *currency;
    char          *localSymbol;
    char          *tradingClass;
    /* pick a non-aggregate (ie not the SMART exchange) exchange that the contract trades on.
     * DO NOT SET TO SMART.*/
    char          *primaryExch;
    bool          includeExpired;   /* can not be set to true for orders.*/

    char          *secIdType;
    char          *secId;
    // COMBOS
    char          *comboLegsDescrip;  /* received in open order version 14 and up for all combos */
    tr_comboleg_t *comboLegs;
    int           comboLegsCount;

    // delta neutral
    under_comp_t  *underComp;
} tr_contract_t;

typedef struct tr_tag_value {
    char *tag;
    char *val;
} tr_tag_value_t;


typedef struct _contract_details {
    tr_contract_t              summary;
    char                      *marketName;
    double                     minTick;
    int                    priceMagnifier;
    char                      *orderTypes;
    char                      *validExchanges;
    int                    underConId;
    char                      *longName;
    char                      *contractMonth;
    char                      *industry;
    char                      *category;
    char                      *subcategory;
    char                      *timeZoneId;
    char                      *tradingHours;
    char                      *liquidHours;
    char                      *evRule;
    double                     evMultiplier;

    tr_tag_value_t            *secIdList;
    int                    secIdListCount;

    // BOND values
    char                      *cusip;
    char                      *ratings;
    char                      *descAppend;
    char                      *bondType;
    char                      *couponType;
    bool                    callable;
    bool                    putable;
    double                     coupon;
    bool                    convertible;
    char                      *maturity;
    char                      *issueDate;
    char                      *nextOptionDate;
    char                      *nextOptionType;
    bool                   nextOptionPartial;
    char                      *notes;
} tr_contract_details_t;

typedef struct tr_order_combo_leg {
    double price;
} tr_order_combo_leg_t;

typedef struct _order {
    // main order fields
    int      orderId;
    int      clientId;
    int      permId;
    char    *action;
    int      totalQuantity;
    char    *orderType;
    double   lmtPrice;
    double   auxPrice;

    // extended order fields
    char    *tif;
    char    *activeStartTime;
    char    *activeStopTime;
    char    *ocaGroup;
    tr_oca_type_t ocaType;
    char    *orderRef;
    bool     transmit;
    int     parentId;
    bool     blockOrder;
    bool     sweepToFill;
    int     displaySize;
    int     triggerMethod;
    bool outsideRth;
    bool hidden;
    char    *goodAfterTime;
    char    *goodTillDate;
    bool overridePercentageConstraints;
    char    *rule80A;
    bool allOrNone;
    int      minQty;
    double   percentOffset;
    double   trailStopPrice;
    double   trailingPercent;

    // Finacial advisors only
    char    *faGroup;
    char    *faProfile;
    char    *faMethod;
    char    *faPercentage;

    // Institutional orders only
    char    *openClose;
    tr_origin_t origin;
    int shortSaleSlot;
    char    *designatedLocation;
    int      exemptCode;

    // SMART routing only
    double   discretionaryAmt;
    bool eTradeOnly;
    bool firmQuoteOnly;
    double   nbboPriceCap;
    bool optOutSmartRouting;

    // BOX or VOL ORDERS ONLY
    tr_auction_strategy_t auctionStrategy;


    // BOX ORDERS ONLY
    double   startingPrice;
    double   stockRefPrice;
    double   delta;

    // pegged to stock or VOL orders
    double   stockRangeLower;
    double   stockRangeUpper;

    // VOLATILITY ORDERS ONLY
    double   volatility;
    int      volatilityType;
    int      continuousUpdate;
    int      referencePriceType;
    char    *deltaNeutralOrderType;
    double   deltaNeutralAuxPrice;
    int      deltaNeutralConId;
    char    *deltaNeutralSettlingFirm;
    char    *deltaNeutralClearingAccount;
    char    *deltaNeutralClearingIntent;
    char    *deltaNeutralOpenClose;
    bool     deltaNeutralShortSale;
    int      deltaNeutralShortSaleSlot;
    char    *deltaNeutralDesignatedLocation;

    // COMBO ORDERS ONLY
    double   basisPoints;
    int      basisPointsType;

    // SCALE ORDER ONLY
    int      scaleInitLevelSize;
    int      scaleSubsLevelSize;
    double   scalePriceIncrement;
    double   scalePriceAdjustValue;
    int      scalePriceAdjustInterval;
    double   scaleProfitOffset;
    bool      scaleAutoReset;
    int      scaleInitPosition;
    int      scaleInitFillQty;
    bool      scaleRandomPercent;
    char    *scaleTable;

    // HEDGE ORDERS ONLY
    char    *hedgeType;
    char    *hedgeParam;

    // Clearing info
    char    *account;
    char    *settlingFirm;
    char    *clearingAccount;
    char    *clearingIntent;

    // ALGO ORDER ONLY
    char    *algoStrategy;
    tr_tag_value_t *algoParams;
    int      algoParamsCount;

    // What-if
    bool whatIf;

    // Not Held
    bool notHeld;

    // Smart combo routing params
    tr_tag_value_t *smartComboRoutingParams;
    int      smartComboRoutingParamsCount;

    // order combo legs
    tr_order_combo_leg_t *comboLegs;
    int      comboLegsCount;
    // order misc options
    tr_tag_value_t *orderMiscOptions;
    int orderMiscOptionsCount;
} tr_order_t;

/*
OrderState
*/
typedef struct _order_state {
    char *status;
    char *initMargin;
    char *maintMargin;
    char *equityWithLoan;
    double commission;
    double minCommission;
    double maxCommission;
    char *commissionCurrency;
    char *warningText;
} tr_order_state_t;

typedef struct _execution {
    int    orderId;
    int    clientId;
    char *execId;
    char *time;
    char *acctNumber;
    char *exchange;
    char *side;
    int    shares;
    double price;
    int    permId;
    int    liquidation;
    int    cumQty;
    double avgPrice;
    char *orderRef;
    char *evRule;
    double evMultiplier;
} tr_execution_t;

typedef struct _exec_filter {
    int   clientId;
    char *acctCode;
    char *time;
    char *symbol;
    char *secType;
    char *exchange;
    char *side;
} tr_exec_filter_t;

typedef struct _scanner_subscription {
    int    numberOfRows;
    char *instrument;
    char *locationCode;
    char *scanCode;
    double abovePrice;
    double belowPrice;
    int    aboveVolume;
    int    averageOptionVolumeAbove;
    double marketCapAbove;
    double marketCapBelow;
    char *moodyRatingAbove;
    char *moodyRatingBelow;
    char *spRatingAbove;
    char *spRatingBelow;
    char *maturityDateAbove;
    char *maturityDateBelow;
    double couponRateAbove;
    double couponRateBelow;
    char *excludeConvertible;
    char *scannerSettingPairs;
    char *stockTypeFilter;
} tr_scanner_subscription_t;

typedef struct _commission_report {
    char *execId;
    double commission;
    char *currency;
    double realizedPNL;
    double yield;
    int    yieldRedemptionDate; /* YYYYMMDD format */
} tr_commission_report_t;

/* Data API */
EXPORT void tws_init_tr_comboleg(tr_comboleg_t *comboleg_ref);
EXPORT void tws_destroy_tr_comboleg(tr_comboleg_t *comboleg_ref);

EXPORT void tws_init_exec_filter(tr_exec_filter_t *filter);
EXPORT void tws_destroy_exec_filter(tr_exec_filter_t *filter);

EXPORT void tws_init_scanner_subscription(tr_scanner_subscription_t *ss);
EXPORT void tws_destroy_scanner_subscription(tr_scanner_subscription_t *ss);

EXPORT void tws_init_tag_value(tr_tag_value_t *t);
EXPORT void tws_destroy_tag_value(tr_tag_value_t *t);

EXPORT void tws_init_order_combo_leg(tr_order_combo_leg_t *ocl);
EXPORT void tws_destroy_order_combo_leg(tr_order_combo_leg_t *ocl);

EXPORT void tws_init_under_comp(under_comp_t *u);
EXPORT void tws_destroy_under_comp(under_comp_t *u);

EXPORT void tws_init_order(tr_order_t *o);
EXPORT void tws_destroy_order(tr_order_t *o);

EXPORT void tws_init_contract(tr_contract_t *c);
EXPORT void tws_destroy_contract(tr_contract_t *c);

EXPORT void tws_init_execution(tr_execution_t *exec);
EXPORT void tws_destroy_execution(tr_execution_t *exec);

EXPORT void tws_init_contract_details(tr_contract_details_t *cd);
EXPORT void tws_destroy_contract_details(tr_contract_details_t *cd);

EXPORT void tws_init_order_state(tr_order_state_t *ost);
EXPORT void tws_destroy_order_state(tr_order_state_t *ost);

#ifdef __cplusplus
}
#endif

#endif  // SKYWALKER_TC_TWS_TWS_TYPE_H

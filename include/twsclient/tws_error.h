#ifndef SKYWALKER_TC_TWS_TWS_ERROR_H
#define SKYWALKER_TC_TWS_TWS_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

/* ERROR codes */
typedef enum tws_client_error_codes {
    NO_VALID_ID = -1,
    NO_TWS_ERROR = 0,
    UNKNOWN_TWS_ERROR = 500,
    ALREADY_CONNECTED = 501,
    CONNECT_FAIL = 502,
    UPDATE_TWS = 503,
    NOT_CONNECTED = 504,
    UNKNOWN_ID = 505,
    ZERO_BYTE_READ = 506,
    NULL_STRING_READ = 507,
    NO_BYTES_READ = 508,
    SOCKET_EXCEPTION = 509,

    FAIL_SEND_REQMKT = 510,
    FAIL_SEND_CANMKT = 511,
    FAIL_SEND_ORDER = 512,
    FAIL_SEND_ACCT = 513,
    FAIL_SEND_EXEC = 514,
    FAIL_SEND_CORDER = 515,
    FAIL_SEND_OORDER = 516,
    UNKNOWN_CONTRACT = 517,
    FAIL_SEND_REQCONTRACT = 518,
    FAIL_SEND_REQMKTDEPTH = 519,
    FAIL_SEND_CANMKTDEPTH = 520,
    FAIL_SEND_SERVER_LOG_LEVEL = 521,
    FAIL_SEND_FA_REQUEST = 522,
    FAIL_SEND_FA_REPLACE = 523,
    FAIL_SEND_REQSCANNER = 524,
    FAIL_SEND_CANSCANNER = 525,
    FAIL_SEND_REQSCANNERPARAMETERS = 526,
    FAIL_SEND_REQHISTDATA = 527,
    FAIL_SEND_CANHISTDATA = 528,
    FAIL_SEND_REQRTBARS = 529,
    FAIL_SEND_CANRTBARS = 530,
    FAIL_SEND_REQCURRTIME = 531,
    FAIL_SEND_REQFUNDDATA = 532,
    FAIL_SEND_CANFUNDDATA = 533,
    FAIL_SEND_REQCALCIMPLIEDVOLAT = 534,
    FAIL_SEND_REQCALCOPTIONPRICE = 535,
    FAIL_SEND_CANCALCIMPLIEDVOLAT = 536,
    FAIL_SEND_CANCALCOPTIONPRICE = 537,
    FAIL_SEND_REQGLOBALCANCEL = 538,
    FAIL_SEND_REQMARKETDATATYPE = 539,
    FAIL_SEND_REQPOSITIONS = 540,
    FAIL_SEND_CANPOSITIONS = 541,
    FAIL_SEND_REQACCOUNTDATA = 542,
    FAIL_SEND_CANACCOUNTDATA = 543,
    FAIL_SEND_VERIFYREQUEST = 544,
    FAIL_SEND_VERIFYMESSAGE = 545,
    FAIL_SEND_QUERYDISPLAYGROUPS = 546,
    FAIL_SEND_SUBSCRIBETOGROUPEVENTS = 547,
    FAIL_SEND_UPDATEDISPLAYGROUP = 548,
    FAIL_SEND_UNSUBSCRIBEFROMGROUPEVENTS = 549,
    FAIL_SEND_STARTAPI = 550,

    FAIL_SEND_BULLETINS = 596,
    FAIL_SEND_CBULLETINS = 597,
    FAIL_SEND_REQIDS = 598,
    FAIL_SEND_EXCERCISE_OPTIONS = 599,

    /* Error messages */
    FAIL_MAX_RATE_OF_MESSAGES_EXCEEDED = 100,
    FAIL_MAX_NUMBER_OF_TICKERS_REACHED = 101,
    FAIL_DUPLICATE_TICKER_ID = 102,
    FAIL_DUPLICATE_ORDER_ID = 103,
    FAIL_MODIFY_FILLED_ORDER = 104,
    FAIL_MODIFIED_ORDER_DOES_NOT_MATCH_ORIGINAL = 105,
    FAIL_TRANSMIT_ORDER_ID = 106,
    FAIL_TRANSMIT_INCOMPLETE_ORDER = 107,
    FAIL_PRICE_OUT_OF_RANGE = 109,
    FAIL_PRICE_DOES_NOT_CONFORM_TO_VARIATION = 110,
    FAIL_TIF_AND_ORDER_TYPE_ARE_INCOMPATIBLE = 111,
    FAIL_ILLEGAL_TIF_OPTION = 113,
    FAIL_RELATIVE_ORDERS_INVALID = 114,
    FAIL_SUBMIT_RELATIVE_ORDERS = 115,
    FAIL_DEAD_EXCHANGE = 116,
    FAIL_BLOCK_ORDER_SIZE = 117,
    FAIL_VWAP_ORDERS_ROUTING = 118,
    FAIL_NON_VWAP_ORDERS_ON_VWAP_EXCHANGE = 119,
    FAIL_VWAP_ORDER_TOO_LATE = 120,
    FAIL_INVALID_BD_FLAG = 121,
    FAIL_NO_REQUEST_TAG = 122,
    FAIL_NO_RECORD_AVAILABLE = 123,
    FAIL_NO_MARKET_RULE_AVAILABLE = 124,
    FAIL_BUY_PRICE_INCORRECT = 125,
    FAIL_SELL_PRICE_INCORRECT = 126,
    FAIL_VWAP_ORDER_PLACED_AFTER_START_TIME = 129,
    FAIL_SWEEP_TO_FILL_INCORRECT = 131,
    FAIL_CLEARING_ACCOUNT_MISSING = 132,
    FAIL_SUBMIT_NEW_ORDER = 133,
    FAIL_MODIFY_ORDER = 134,
    FAIL_FIND_ORDER = 135,
    FAIL_ORDER_CANNOT_CANCEL = 136,
    FAIL_CANNOT_CANCEL_VWAP_ORDER = 137,
    FAIL_COULD_NOT_PARSE_TICKER_REQUEST = 138,
    FAIL_PARSING_ERROR = 139,
    FAIL_SIZE_VALUE_NOT_INTEGER = 140,
    FAIL_PRICE_VALUE_NOT_FLTPOINT = 141,
    FAIL_MISSING_INSTITUTIONAL_ACCOUNT_INFO = 142,
    FAIL_REQUESTED_ID_NOT_INTEGER = 143,
    FAIL_ORDER_SIZE_DOES_NOT_MATCH_ALLOCATION = 144,
    FAIL_INVALID_ENTRY_FIELDS = 145,
    FAIL_INVALID_TRIGGER_METHOD = 146,
    FAIL_CONDITIONAL_CONTRACT_INFO_INCOMPLETE = 147,
    FAIL_CANNOT_SUBMIT_CONDITIONAL_ORDER = 148,
    FAIL_ORDER_TRANSMITTED_WITHOUT_USER_NAME = 151,
    FAIL_INVALID_HIDDEN_ORDER_ATTRIBUTE = 152,
    FAIL_EFPS_CAN_ONLY_LIMIT_ORDERS = 153,
    FAIL_ORDERS_TRANSMITTED_FOR_HALTED_SECURITY = 154,
    FAIL_SIZEOP_ORDER_MUST_HAVE_USERNAME_AND_ACCOUNT = 155,
    FAIL_SIZEOP_ORDER_MUST_GO_TO_IBSX = 156,
    FAIL_ICEBERG_OR_DISCRETIONARY = 157,
    FAIL_MUST_SPECIFY_OFFSET = 158,
    FAIL_PERCENT_OFFSET_VALUE_OUT_OF_RANGE = 159,
    FAIL_SIZE_VALUE_ZERO = 160,
    FAIL_ORDER_NOT_IN_CANCELLABLE_STATE = 161,
    FAIL_HISTORICAL_MARKET_DATA_SERVICE = 162,
    FAIL_VIOLATE_PERCENTAGE_IN_ORDER_SETTINGS = 163,
    FAIL_NO_MARKET_DATA_TO_CHECK_VIOLATIONS = 164,
    INFO_HISTORICAL_MARKET_DATA_SERVICE_QUERY = 165,
    FAIL_HMDS_EXPIRED_CONTRACT_VIOLATION = 166,
    FAIL_VWAP_ORDER_NOT_IN_FUTURE = 167,
    FAIL_DISCRETIONARY_AMOUNT_MISMATCH = 168,

    FAIL_NO_SECURITY_DEFINITION = 200,
    FAIL_ORDER_REJECTED = 201,
    FAIL_ORDER_CANCELED = 202,
    FAIL_SECURITY_NOT_AVAILABLE_ALLOWED = 203,

    FAIL_FIND_EID = 300,
    FAIL_INVALID_TICKER_ACTION = 301,
    FAIL_ERROR_PARSING_STOP = 302,
    FAIL_INVALID_ACTION = 303,
    FAIL_INVALID_ACCOUNT_VALUE = 304,
    FAIL_REQUEST_PARSING_ERROR = 305,
    FAIL_DDE_REQUEST = 306,
    FAIL_INVALID_REQUEST_TOPIC = 307,
    FAIL_UNABLE_TO_CREATE_PAGE = 308,
    FAIL_MAX_NUMBER_MARKET_DEPTH_REQUESTS_REACHED = 309,
    FAIL_FIND_SUBSCRIBED_MARKET_DEPTH = 310,
    FAIL_ORIGIN_INVALID = 311,
    FAIL_COMBO_DETAILS_INVALID = 312,
    FAIL_COMBO_DETAILS_FOR_LEG_INVALID = 313,
    FAIL_SECURITY_TYPE_REQUIRES_COMBO_LEG_DETAILS = 314,
    FAIL_COMBO_LEGS_ROUTING_RESTRICTED = 315,
    FAIL_MARKET_DEPTH_DATA_HALTED = 316,
    FAIL_MARKET_DEPTH_DATA_RESET = 317,
    FAIL_INVALID_LOG_LEVEL = 319,
    FAIL_SERVER_ERROR_READING_REQUEST = 320,
    FAIL_SERVER_ERROR_VALIDATING_REQUEST = 321,
    FAIL_SERVER_ERROR_PROCESSING_REQUEST = 322,
    FAIL_SERVER_ERROR = 323,
    FAIL_SERVER_ERROR_READING_DDE_REQUEST = 324,
    FAIL_DISCRETIONARY_ORDERS_NOT_SUPPORTED = 325,
    FAIL_UNABLE_TO_CONNECT_ID_IN_USE = 326,
    FAIL_CANNOT_SET_AUTO_BIND_PROPERTY = 327,
    FAIL_CANNOT_ATTACH_TRAILING_STOP_ORDERS = 328,
    FAIL_ORDER_MODIFY_FAILED = 329,
    FAIL_ONLY_FA_OR_STL_CUSTOMERS = 330,
    FAIL_FA_OR_STL_INTERNAL_ERROR = 331,
    FAIL_INVALID_ACCOUNT_CODES_FOR_PROFILE = 332,
    FAIL_INVALID_SHARE_ALLOCATION_SYNTAX = 333,
    FAIL_INVALID_GOOD_TILL_DATE_ORDER = 334,
    FAIL_DELTA_OUT_OF_RANGE = 335,
    FAIL_TIME_OR_TIME_ZONE_INVALID = 336,
    FAIL_DATE_TIME_INVALID = 337,
    FAIL_GOOD_AFTER_TIME_ORDERS_DISABLED = 338,
    FAIL_FUTURES_SPREAD_NOT_SUPPORTED = 339,
    FAIL_INVALID_IMPROVEMENT_AMOUNT = 340,
    FAIL_INVALID_DELTA = 341,
    FAIL_PEGGED_ORDER_NOT_SUPPORTED = 342,
    FAIL_DATE_TIME_ENTERED_INVALID = 343,
    FAIL_ACCOUNT_NOT_FINANCIAL_ADVISOR = 344,
    FAIL_GENERIC_COMBO_NOT_SUPPORTED = 345,
    FAIL_NOT_PRIVILEGED_ACCOUNT = 346,
    FAIL_SHORT_SALE_SLOT_VALUE = 347,
    FAIL_ORDER_NOT_SHORT_SALE = 348,
    FAIL_COMBO_DOES_NOT_SUPPORT_GOOD_AFTER = 349,
    FAIL_MINIMUM_QUANTITY_NOT_SUPPORTED = 350,
    FAIL_REGULAR_TRADING_HOURS_ONLY = 351,
    FAIL_SHORT_SALE_SLOT_REQUIRES_LOCATION = 352,
    FAIL_SHORT_SALE_SLOT_REQUIRES_NO_LOCATION = 353,
    FAIL_NOT_SUBSCRIBED_TO_MARKET_DATA = 354,
    FAIL_ORDER_SIZE_DOES_NOT_CONFORM_RULE = 355,
    FAIL_SMART_COMBO_ORDER_NOT_SUPPORT_OCA = 356,
    FAIL_CLIENT_VERSION_OUT_OF_DATE = 357,
    FAIL_COMBO_CHILD_ORDER_NOT_SUPPORTED = 358,
    FAIL_COMBO_ORDER_REDUCED_SUPPORT = 359,
    FAIL_NO_WHATIF_CHECK_SUPPORT = 360,
    FAIL_INVALID_TRIGGER_PRICE = 361,
    FAIL_INVALID_ADJUSTED_STOP_PRICE = 362,
    FAIL_INVALID_ADJUSTED_STOP_LIMIT_PRICE = 363,
    FAIL_INVALID_ADJUSTED_TRAILING_AMOUNT = 364,
    FAIL_NO_SCANNER_SUBSCRIPTION_FOUND = 365,
    FAIL_NO_HISTORICAL_DATA_QUERY_FOUND = 366,
    FAIL_VOLATILITY_TYPE = 367,
    FAIL_REFERENCE_PRICE_TYPE = 368,
    FAIL_VOLATILITY_ORDERS_US_ONLY = 369,
    FAIL_DYNAMIC_VOLATILITY_ORDERS_ROUTING = 370,
    FAIL_VOL_ORDER_REQUIRES_POSITIVE_VOLATILITY = 371,
    FAIL_DYNAMIC_VOL_ATTRIBUTE_ON_NON_VOL_ORDER = 372,
    FAIL_CANNOT_SET_STOCK_RANGE_ATTRIBUTE = 373,
    FAIL_STOCK_RANGE_ATTRIBUTES_INVALID = 374,
    FAIL_STOCK_RANGE_ATTRIBUTES_NEGATIVE = 375,
    FAIL_ORDER_NOT_ELIGIBLE_FOR_CONTINUOUS_UPDATE = 376,
    FAIL_MUST_SPECIFY_VALID_DELTA_HEDGE_PRICE = 377,
    FAIL_DELTA_HEDGE_REQUIRES_AUX_PRICE = 378,
    FAIL_DELTA_HEDGE_REQUIRES_NO_AUX_PRICE = 379,
    FAIL_ORDER_TYPE_NOT_ALLOWED = 380,
    FAIL_DDE_DLL_NEEDS_TO_UPGRADED = 381,
    FAIL_PRICE_VIOLATES_TICKS_CONSTRAINT = 382,
    FAIL_SIZE_VIOLATES_SIZE_CONSTRAINT = 383,
    FAIL_INVALID_DDE_ARRAY_REQUEST = 384,
    FAIL_DUPLICATE_TICKER_ID_FOR_SCANNER = 385,
    FAIL_DUPLICATE_TICKER_ID_FOR_HISTORICAL_DATA = 386,
    FAIL_UNSUPPORTED_ORDER_TYPE_FOR_EXCHANGE = 387,
    FAIL_ORDER_SIZE_BELOW_REQUIREMENT = 388,
    FAIL_ROUTED_ORDER_ID_NOT_UNIQUE = 389,
    FAIL_ROUTED_ORDER_ID_INVALID = 390,
    FAIL_TIME_OR_TIME_ZONE_ENTERED_INVALID = 391,
    FAIL_INVALID_ORDER_CONTRACT_EXPIRED = 392,
    FAIL_SHORT_SALE_SLOT_FOR_DELTA_HEDGE_ONLY = 393,
    FAIL_INVALID_PROCESS_TIME = 394,
    FAIL_OCA_GROUPS_CURRENTLY_NOT_ACCEPTED = 395,
    FAIL_ONLY_MARKET_AND_LIMIT_ORDERS = 396,
    FAIL_ONLY_MARKET_AND_LIMIT_ORDERS_SUPPORT = 397,
    FAIL_CONDITION_TRIGGER = 398,
    FAIL_ORDER_MESSAGE = 399,

    FAIL_ALGO_ORDER_ERROR = 400,
    FAIL_LENGTH_RESTRICTION = 401,
    FAIL_CONDITIONS_NOT_ALLOWED_FOR_CONTRACT = 402,
    FAIL_INVALID_STOP_PRICE = 403,
    FAIL_SHARES_NOT_AVAILABLE = 404,
    FAIL_CHILD_ORDER_QUANTITY_INVALID = 405,
    FAIL_CURRENCY_NOT_ALLOWED = 406,
    FAIL_INVALID_SYMBOL = 407,
    FAIL_INVALID_SCALE_ORDER_INCREMENT = 408,
    FAIL_INVALID_SCALE_ORDER_COMPONENT_SIZE = 409,
    FAIL_INVALID_SUBSEQUENT_COMPONENT_SIZE = 410,
    FAIL_OUTSIDE_REGULAR_TRADING_HOURS = 411,
    FAIL_CONTRACT_NOT_AVAILABLE = 412,
    FAIL_WHAT_IF_ORDER_TRANSMIT_FLAG = 413,
    FAIL_MARKET_DATA_SUBSCRIPTION_NOT_APPLICABLE = 414,
    FAIL_WAIT_UNTIL_PREVIOUS_RFQ_FINISHES = 415,
    FAIL_RFQ_NOT_APPLICABLE = 416,
    FAIL_INVALID_INITIAL_COMPONENT_SIZE = 417,
    FAIL_INVALID_SCALE_ORDER_PROFIT_OFFSET = 418,
    FAIL_MISSING_INITIAL_COMPONENT_SIZE = 419,
    FAIL_INVALID_REAL_TIME_QUERY = 420,
    FAIL_INVALID_ROUTE = 421,
    FAIL_ACCOUNT_AND_CLEARING_ATTRIBUTES = 422,
    FAIL_CROSS_ORDER_RFQ_EXPIRED = 423,
    FAIL_FA_ORDER_REQUIRES_ALLOCATION = 424,
    FAIL_FA_ORDER_REQUIRES_MANUAL_ALLOCATIONS = 425,
    FAIL_ACCOUNTS_LACK_SHARES = 426,
    FAIL_MUTUAL_FUND_REQUIRES_MONETARY_VALUE = 427,
    FAIL_MUTUAL_FUND_SELL_REQUIRES_SHARES = 428,
    FAIL_DELTA_NEUTRAL_ORDERS_NOT_SUPPORTED = 429,
    FAIL_FUNDAMENTALS_DATA_NOT_AVAILABLE = 430,
    FAIL_WHAT_TO_SHOW_FIELD = 431,
    FAIL_COMMISSION_NEGATIVE = 432,
    FAIL_INVALID_RESTORE_SIZE = 433,
    FAIL_ORDER_SIZE_ZERO = 434,
    FAIL_MUST_SPECIFY_ACCOUNT = 435,
    FAIL_MUST_SPECIFY_ALLOCATION = 436,
    FAIL_ORDER_TOO_MANY_FLAGS = 437,
    FAIL_APPLICATION_LOCKED = 438,
    FAIL_ALGORITHM_DEFINITION_NOT_FOUND = 439,
    FAIL_ALGORITHM_MODIFIED = 440,
    FAIL_ALGO_ATTRIBUTES_VALIDATION_FAILED = 441,
    FAIL_ALGORITHM_NOT_ALLOWED = 442,
    FAIL_UNKNOWN_ALGO_ATTRIBUTE = 443,
    FAIL_VOLATILITY_COMBO_ORDER_SUBMIT_CHANGES = 444,
    FAIL_RFQ_NO_LONGER_VALID = 445,
    FAIL_MISSING_SCALE_ORDER_PROFIT_OFFSET = 446,
    FAIL_MISSING_SCALE_PRICE_ADJUSTMENT = 447,
    FAIL_INVALID_SCALE_PRICE_ADJUSTMENT_INTERVAL = 448,
    FAIL_UNEXPECTED_SCALE_PRICE_ADJUSTMENT = 449,
    FAIL_DIVIDEND_SCHEDULE_QUERY = 450,

    /* System Message Codes */
    FAIL_IB_TWS_CONNECTIVITY_LOST = 1100,
    FAIL_IB_TWS_CONNECTIVITY_RESTORED = 1101,
    FAIL_IB_TWS_CONNECTIVITY_RESTORED_WITH_DATA = 1102,
    FAIL_TWS_SOCKET_PORT_RESET = 1300,

    /* Warning Message Codes */
    FAIL_NEW_ACCOUNT_DATA_REQUESTED = 2100,
    FAIL_UNABLE_TO_SUBSCRIBE_TO_ACCOUNT = 2101,
    FAIL_UNABLE_TO_MODIFY_ORDER = 2102,
    FAIL_MARKET_DATA_FARM_DISCONNECTED = 2103,
    FAIL_MARKET_DATA_FARM_CONNECTED = 2104,
    FAIL_HISTORICAL_DATA_FARM_DISCONNECTED = 2105,
    FAIL_HISTORICAL_DATA_FARM_CONNECTED = 2106,
    FAIL_HISTORICAL_DATA_FARM_INACTIVE = 2107,
    FAIL_MARKET_DATA_FARM_CONNECTION_INACTIVE = 2108,
    FAIL_ORDER_EVENT_ATTRIBUTE_IGNORED = 2109,
    FAIL_IB_TWS_CONNECTIVITY_BROKEN = 2110,
} tws_client_error_code_t;

#ifdef __cplusplus
}
#endif

#endif  // SKYWALKER_TC_TWS_TWS_ERROR_H

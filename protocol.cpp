#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/print.hpp>
#include "protocol.hpp"
using namespace eosio;

struct impl {


void start_new_cycle ( account_name username, account_name host){
        
        cycle_index cycles(_self, host);
        pool_index pools(_self, host);
        dprop_index dprops(_self, host);
        rate_index rates(_self, host);
        spiral_index spiral(_self, host);
        account_index accounts(_self, _self);
        sincome_index sincomes(_self, host);

        auto dprop = dprops.find(0);
        auto pool = pools.find(dprop -> current_pool_id);
        auto cycle = cycles.find(dprop-> current_cycle_num - 1);
        auto rate = rates.find(0);
        auto next_rate = rates.find(1);

        auto sp = spiral.find(0);

        if ((pool-> pool_num > 2) && (pool->pool_num < sp->pool_limit - 1)){
            auto prev_rate = rates.find(pool->pool_num - 1);
            auto sinc = sincomes.find(dprop->current_cycle_num - 1);

            sincomes.modify(sinc, username, [&](auto &s){
                s.pool_num = pool -> pool_num;
                s.available = prev_rate -> system_income;
            });
        }

        cycles.emplace(username, [&](auto &c){
            c.id = cycles.available_primary_key();
            c.start_at_global_pool_id = dprop->current_pool_id + 1; 
            c.finish_at_global_pool_id = dprop->current_pool_id + 1;    
        });

        dprops.modify(dprop, username, [&](auto &dp){
            dp.current_pool_id  = dprop->current_pool_id + 1;
            dp.cycle_start_at_id = dp.current_pool_id;
            dp.current_cycle_num = dprop->current_cycle_num + 1;
            dp.current_pool_num  = 1;
                  
        });
        
        pools.emplace(username, [&](auto &p){
            p.total_lepts = sp->size_of_pool * LEPTS_PRECISION;
            p.id = pools.available_primary_key();
            p.total_reserved_lepts = 0;
            p.remain_lepts = sp -> size_of_pool * LEPTS_PRECISION;
            p.lept_cost = asset(rate->buy_rate, _SYM);
            p.released_lepts = 0;
            p.cycle_num = pool->cycle_num + 1;
            p.next_pool = p.remain_lepts * p.lept_cost / LEPTS_PRECISION;
            p.pool_num = 1;
            p.color = p.id % 2 == 0 ? "black" : "white"; 
            p.pool_start_at = eosio::time_point_sec(now());
            p.pool_expired_at = eosio::time_point_sec (now() + sp->pool_timeout);
            p.total_in_box = asset(0, _SYM);
            p.total_win_withdraw = asset(0, _SYM);
            p.total_loss_withdraw = asset(0, _SYM);
        });

        
        sincomes.emplace(username, [&](auto &sinc){
            sinc.id = sincomes.available_primary_key();
            sinc.pool_num = 0;
            sinc.available = asset(0, _SYM);
        });

        refresh_state(username, host);  
    };

    void next_pool( account_name username, account_name host){
        cycle_index cycles(_self, host);
        pool_index pools(_self, host);
        dprop_index dprops(_self, host);
        rate_index rates(_self, host);
        spiral_index spiral(_self, host);
        sincome_index sincomes(_self, host);

        auto dprop = dprops.find(0);
        auto pool = pools.find(dprop -> current_pool_id);
        auto cycle = cycles.find(dprop -> current_cycle_num - 1);
        auto rate = rates.find(dprop-> current_pool_num);

        auto sp = spiral.find(0);
        
        uint128_t dreserved_lepts = 0;
        uint64_t reserved_lepts = 0;

        cycles.modify(cycle, username, [&](auto &c ){
            c.finish_at_global_pool_id = cycle -> finish_at_global_pool_id + 1;
        });

        dprops.modify(dprop, username, [&](auto &dp){
           dp.current_pool_num = pool -> pool_num + 1;
           dp.current_pool_id  = pool -> id + 1;
        });

        if (dprop -> current_pool_num > 2) {
            
            auto prev_prev_pool = pools.find(dprop -> current_pool_id - 2);
            
            dreserved_lepts = (prev_prev_pool -> released_lepts + prev_prev_pool -> total_reserved_lepts) / LEPTS_PRECISION  * rate -> sell_rate / rate -> buy_rate  \
              * LEPTS_PRECISION;

            reserved_lepts = uint64_t(dreserved_lepts);
            
        };


        if ((pool-> pool_num > 2) && (pool->pool_num < sp->pool_limit )){
            auto sinc = sincomes.find(dprop->current_cycle_num - 1);
            
            sincomes.modify(sinc, username, [&](auto &s){
                s.pool_num = pool -> pool_num;
                s.available = rate -> system_income;
            });
        }
        
        pools.emplace(username, [&](auto &p){
            p.id = pools.available_primary_key();
            p.total_lepts = sp->size_of_pool * LEPTS_PRECISION;
            p.total_reserved_lepts = reserved_lepts;
            p.remain_lepts = p.total_lepts - reserved_lepts;
            p.lept_cost = asset(rate->buy_rate, _SYM);
            p.released_lepts = 0;
            p.color = p.id % 2 == 0 ? "black" : "white"; 
            p.cycle_num = pool -> cycle_num;
            p.pool_num = pool -> pool_num + 1;
            p.next_pool = pool -> total_in_box + p.remain_lepts * p.lept_cost / LEPTS_PRECISION;
            p.total_in_box = pool-> total_in_box;
            p.pool_start_at = eosio::time_point_sec(now());
            p.pool_expired_at = eosio::time_point_sec (now() + sp->pool_timeout);
            p.total_win_withdraw = asset(0, _SYM);
            p.total_loss_withdraw = asset(0, _SYM);
        });
    };



    void start_action (const start &op){
        require_auth(op.host);

        account_index accounts(_self, _self);
        auto account = accounts.find(op.host);
        eosio_assert(account != accounts.end(), "Account is not upgraded to Host. Please update your account and try again.");
        
        spiral_index spiral(_self, op.host);
        auto sp = spiral.find(0);

        eosio_assert(sp != spiral.end(), "Protocol is not found. Set parameters at first");
        eosio_assert(! account -> activated, "Protocol is already active and cannot be activated again");
        
        accounts.modify(account, op.host, [&](auto &a){
            a.activated = true;
        });

        cycle_index cycles(_self, op.host);
        pool_index pools(_self, op.host);
        dprop_index dprops(_self, op.host);
        rate_index rates(_self, op.host);

        auto rate = rates.find(0);
        
        pools.emplace(op.host, [&](auto &p){
            p.id = 0;
            p.total_lepts = sp->size_of_pool * LEPTS_PRECISION;
            p.total_reserved_lepts = 0;
            p.remain_lepts = p.total_lepts;
            p.lept_cost = asset(rate->buy_rate, _SYM);
            p.total_in_box = asset(0, _SYM);
            p.total_win_withdraw = asset(0, _SYM);
            p.total_loss_withdraw = asset(0, _SYM);
            p.released_lepts = 0;
            p.next_pool = p.remain_lepts * p.lept_cost / LEPTS_PRECISION;
            p.pool_num = 1;
            p.cycle_num = 1;
            p.pool_start_at = eosio::time_point_sec(now());
            p.pool_expired_at = eosio::time_point_sec (now() + sp->pool_timeout);
            p.color = "black";
        });
        
        dprops.emplace(op.host, [&](auto &dp){}); 

        cycles.emplace(op.host, [&](auto &c){
            c.id = cycles.available_primary_key();
            c.start_at_global_pool_id = 0;
        });

        sincome_index sincomes(_self, op.host);

        sincomes.emplace(op.host, [&](auto &sinc){
            sinc.id = 0;
            sinc.pool_num = 0;
            sinc.available = asset(0, _SYM);
        });        
    }


    void setfee_action(const setfee &op){
        require_auth(_dacomfee);
    
        fee_index fee(_self,_self);

        auto fe = fee.find(0);
        if (fe == fee.end()){
            fee.emplace(_dacomfee, [&](auto &f){
                f.createhost = op.createhost;
                f.systemfee = op.systemfee;
            });
        } else {
     
           fee.modify(fe, _dacomfee, [&](auto &f){
                f.createhost = op.createhost;
                f.systemfee = op.systemfee;
            });
        }
    }

    void setparams_action(const setparams &op){
        require_auth(op.host);
        
        account_index accounts(_self, _self);
        rate_index rates(_self, op.host);
        spiral_index spiral(_self, op.host);
        
        auto account = accounts.find(op.host);
        eosio_assert(account != accounts.end(), "Account is not upgraded to Host. Please update your account and try again.");
        eosio_assert(account->activated == false, "Protocol is already active and cannot be changed now.");

        auto size_of_pool = op.size_of_pool;
        auto overlap = op.overlap;
        auto profit_growth = op.profit_growth;
        auto base_rate = op.base_rate;
        auto loss_percent = op.loss_percent;
        auto pool_limit = op.pool_limit;
        auto pool_timeout = op.pool_timeout;

        eosio_assert((overlap > 10000) && (overlap < 20000), "Overlap factor must be greater then 10000 (1.0000) and less then 20000 (2.0000)).");
        eosio_assert(profit_growth <= 20000, "Profit growth factor must be greater or equal 0 (0.0000) and less then 20000 (2.0000)).");
        eosio_assert((loss_percent > 0 ) && ( loss_percent <= 10000), "Loss Percent must be greater then 0 (0%) and less or equal 10000 (100%)");
        eosio_assert((base_rate >= 10) && (base_rate < 1000000000), "Base Rate must be greater or equal 10 and less then 1e9");
        eosio_assert((size_of_pool >= 10) && (size_of_pool < 1000000000), "Size of Pool must be greater or equal 10 and less then 1e9");
        eosio_assert((pool_limit >= 4) && (pool_limit < 1000), "Pool Count must be greater or equal 4 and less or equal 1000");
        eosio_assert((pool_timeout >= 60) && (pool_timeout < 7884000),"Pool Timeout must be greater or equal then 60 sec and less then 7884000 sec");
        
        auto sp = spiral.find(0);
        auto rate = rates.find(0);
        
        if (((sp != spiral.end()) && rate != rates.end()) && (account -> activated == false)){
            spiral.erase(sp);
            auto it = rates.begin();
            while (it != rates.end()) {
                it = rates.erase(it);
            };
        };

        spiral.emplace(op.host, [&](auto &s){
            s.overlap = overlap;
            s.size_of_pool = size_of_pool;
            s.profit_growth = profit_growth;
            s.base_rate = base_rate;
            s.loss_percent = loss_percent;
            s.pool_limit = pool_limit;
            s.pool_timeout = pool_timeout;

        });
        
        uint64_t buy_rate[pool_limit];
        uint64_t sell_rate[pool_limit];
        uint64_t delta[pool_limit];
        uint64_t client_income[pool_limit];
        uint64_t pool_cost[pool_limit];
        uint64_t total_in_box[pool_limit];
        uint64_t payment_to_wins[pool_limit];
        uint64_t payment_to_loss[pool_limit];
        uint64_t system_income[pool_limit];
        uint64_t live_balance_for_sale[pool_limit];
        
        for (auto i=0; i < pool_limit - 1; i++ ){
             
             if (i == 0){
                buy_rate[i] = base_rate;
                sell_rate[i] = base_rate;
                delta[i] = 0;
                delta[i+1] = 0;
                client_income[i] = 0;
                pool_cost[i] = size_of_pool * buy_rate[i];
                total_in_box[i] = pool_cost[i];
                payment_to_wins[i] = 0;
                payment_to_loss[i] = 0;
                system_income[i] = 0;
                live_balance_for_sale[i] = total_in_box[i];

            } else if (i > 0){ 
                sell_rate[i + 1] = buy_rate[i-1] * overlap / PERCENT_PRECISION;
                client_income[i+1] = sell_rate[i+1] - buy_rate[i-1];
                delta[i+1] = i > 1 ? client_income[i+1] * profit_growth / PERCENT_PRECISION + delta[i] : client_income[i+1];
                buy_rate[i] = sell_rate[i+1] + delta[i+1];
                pool_cost[i] = size_of_pool * buy_rate[i];
                payment_to_wins[i] = i > 1 ? size_of_pool * sell_rate[i] : 0;
                live_balance_for_sale[i] = pool_cost[i] - payment_to_wins[i];
                
                if (i == 1){
                    sell_rate[i] = buy_rate[i];
                    payment_to_loss[i] = 0;
                
                } else if (i == 2){
                    payment_to_loss[i] = live_balance_for_sale[i-1] * (PERCENT_PRECISION - loss_percent) / PERCENT_PRECISION;
                } else if ( i == 3 ){
                    payment_to_loss[i] = live_balance_for_sale[i-2] * (PERCENT_PRECISION - loss_percent) / PERCENT_PRECISION + live_balance_for_sale[i-1] * (PERCENT_PRECISION - loss_percent) / PERCENT_PRECISION;
                } else if ( i > 3 ){
                    payment_to_loss[i] = live_balance_for_sale[i-1] * (PERCENT_PRECISION - loss_percent) / PERCENT_PRECISION + payment_to_loss[i - 2];
                }
                
                system_income[i] = i > 1 ? total_in_box[i-1] - payment_to_wins[i] - payment_to_loss[i] : 0; 
                total_in_box[i] = i > 1 ? total_in_box[i-1] + live_balance_for_sale[i] : total_in_box[i-1] + pool_cost[i];

                bool positive = total_in_box[i-1] - payment_to_wins[i] <= payment_to_loss[i] ? false : true;
                
                eosio_assert(positive, "The financial model of Protocol is Negative. Try with a different parameters");

                eosio_assert(total_in_box[i-1] < _MAX_SUPPLY, "Tokens in the Box are exceeds the max supply. Try to decrease Protocol parameters.");
                
                if (i > 2)
                    eosio_assert((client_income[i-1] > 0), "Overlap rate is too small for Protocol operation. Try to increase Overlap parameter, Size of Pool or Base Rate");      
            } 
        };


        for (auto i = 0; i < pool_limit - 1; i++){
            rates.emplace(op.host, [&](auto &r){
                    r.pool_id = i;
                    r.buy_rate = buy_rate[i];
                    r.sell_rate = sell_rate[i];
                    r.client_income = asset(client_income[i], _SYM);
                    r.delta = asset(delta[i], _SYM);
                    r.total_lepts = size_of_pool;
                    r.pool_cost = asset(pool_cost[i], _SYM);
                    r.payment_to_wins = asset(payment_to_wins[i], _SYM);
                    r.payment_to_loss = asset(payment_to_loss[i], _SYM);
                    r.total_in_box = asset(total_in_box[i], _SYM);
                    r.system_income = asset(system_income[i], _SYM);
                    r.live_balance_for_sale = asset(live_balance_for_sale[i], _SYM);
                });
        }
    }


    
    void deposit ( account_name username, account_name host, eosio::asset amount){
        require_auth(username);

        eosio_assert( amount.is_valid(), "Rejected. Invalid quantity" );
        eosio_assert ( amount.symbol == _SYM, "Rejected. Invalid symbol for this contract.");
        
        refresh_state(username, host);

        rate_index rates(_self, host);
        pool_index pools(_self, host);
        dprop_index dprops(_self, host);
        account_index accounts(_self, _self);
        
        auto acc = accounts.find(host);
        eosio_assert(acc != accounts.end(), "Rejected. Host is not founded.");
        eosio_assert(acc -> activated == true, "Rejected. Protocol is not active");

        auto dprop = dprops.find(0);
        auto pool = pools.find( dprop-> current_pool_id );
        eosio_assert(pool->remain_lepts <= pool->total_lepts, "System Error, not possible to deposit in this branch");
        eosio_assert ( pool -> pool_expired_at > eosio::time_point_sec(now()), "Rejected. Pool is Expired. Need manual refresh.");
        
        auto rate = rates.find( pool-> pool_num - 1 );
        
        eosio_assert(amount.amount % rate -> buy_rate == 0, "You can purchase only whole pieces of LEPT. Specify the amount of the multiple purchase rate.");

        uint64_t lepts = LEPTS_PRECISION * amount.amount / rate -> buy_rate;

        eosio_assert(pool -> remain_lepts >= lepts, "Not enought LEPT in target pool");
        
        fill_pool(username, host, lepts, amount);
    
        refresh_state(username, host);
        print("Successful deposit in the DACom protocol");
    };



    void refresh_state (account_name username, account_name host){
       
        pool_index pools(_self, host);
        dprop_index dprops(_self, host);
        spiral_index spiral(_self, host);

        auto sp = spiral.find(0);
        auto dprop = dprops.find(0);
        auto pool = pools.find(dprop -> current_pool_id);
        
        if ((pool -> pool_expired_at < eosio::time_point_sec(now()) || \
            ((pool -> pool_num + 1 == sp->pool_limit) && (pool -> remain_lepts == 0)))){
                
            start_new_cycle(username, host);
      
        } else if ((pool -> remain_lepts == 0)){
      
            next_pool(username, host);
      
        }
        
    };


    void fill_pool(account_name username, account_name host, uint64_t lepts, eosio::asset amount){
        std::vector<eosio::asset> forecasts;
        
        cycle_index cycles(_self, host);
        pool_index pools(_self, host);
        dprop_index dprops(_self, host);
        balance_index balance(_self, username);

        auto dprop = dprops.find(0);
        auto pool = pools.find(dprop->current_pool_id);
        
        uint64_t remain_lepts = pool -> remain_lepts - lepts;
        uint64_t released_lepts = pool -> released_lepts + lepts;
     
        forecasts = calculate_forecast(username, host, lepts, pool -> pool_num - 1);

        pools.modify(pool, username, [&](auto &p){
            p.remain_lepts = remain_lepts;
            p.released_lepts = released_lepts;
            p.total_in_box = pool-> total_in_box + amount;
        });

        balance.emplace(username, [&](auto &b){
            b.id = balance.available_primary_key();
            b.cycle_num = pool->cycle_num;
            b.pool_num = pool->pool_num;
            b.host = host;
            b.global_pool_id = dprop->current_pool_id; 
            b.pool_color = pool -> color;
            b.lept_for_sale = lepts;
            b.purchase_amount = amount;
            b.available = amount;
            b.date_of_purchase = eosio::time_point_sec(now());
            b.last_recalculated_win_pool_id = pool -> id;
            b.forecasts = forecasts;
            b.sold_amount = asset(0, _SYM);
        });
    };




    void refresh_balance_action (const refreshbal &op){
        require_auth(op.username);
        account_name username = op.username;
        uint64_t balance_id = op.balance_id;

        std::vector<eosio::asset> forecasts;
        account_index accounts(_self, _self);
        balance_index balance (_self, username);
        auto bal = balance.find(balance_id);
        eosio_assert(bal != balance.end(), "Balance is not exist or already withdrawed");
        
        auto host = bal -> host;

        refresh_state(username, host);
        
        cycle_index cycles(_self, host);
        rate_index rates(_self, host);
        pool_index pools(_self, host);
        dprop_index dprops(_self, host);

        
        auto pool_start = pools.find(bal -> global_pool_id);
        auto cycle = cycles.find(pool_start -> cycle_num - 1);
        auto next_cycle = cycles.find(pool_start -> cycle_num);
        auto pools_in_cycle = cycle -> finish_at_global_pool_id - cycle -> start_at_global_pool_id + 1;
        auto account = accounts.find(username);
        auto dprop = dprops.find(0);
        auto last_pool = pools.find(cycle -> finish_at_global_pool_id );
        auto has_new_cycle = false;

        if (next_cycle != cycles.end())
            has_new_cycle = true;

        uint64_t ceiling;
        if (( op.partrefresh == true )&&(bal -> last_recalculated_win_pool_id + 10 < cycle -> finish_at_global_pool_id)){
            ceiling = bal -> last_recalculated_win_pool_id + 10;

        } else {
            ceiling = cycle -> finish_at_global_pool_id;
        }


        if (bal -> last_recalculated_win_pool_id <= cycle -> finish_at_global_pool_id)
            for (auto i = bal -> last_recalculated_win_pool_id + 1;  i <= ceiling; i++){
                auto look_pool = pools.find(i);
                auto purchase_amount = bal-> purchase_amount;
                
                if (((dprop -> current_pool_num == pool_start -> pool_num ) && (dprop -> current_cycle_num == pool_start -> cycle_num)) || \
                    ((pool_start -> pool_num < 3) && (pools_in_cycle < 3)) || (has_new_cycle && (pool_start->pool_num == last_pool -> pool_num)))

                {
                    balance.modify(bal, username, [&](auto &b){
                          b.last_recalculated_win_pool_id = i;
                          b.available = purchase_amount;
                    });

                } else {
                    
                    eosio::asset available;
                    uint64_t new_reduced_lepts;
                    uint64_t new_lept_for_sale;

                    if (pool_start -> color == look_pool -> color){
                        auto rate = rates.find(look_pool -> pool_num - 1);

                        if (look_pool -> pool_num - pool_start -> pool_num <= 2){
                            
                            new_reduced_lepts = bal -> lept_for_sale * rate -> sell_rate / rate -> buy_rate;
                            new_lept_for_sale = bal->lept_for_sale;
                            forecasts = calculate_forecast(username, host, new_reduced_lepts, look_pool -> pool_num - 1);
                            available = asset(bal ->lept_for_sale * rate -> sell_rate / LEPTS_PRECISION, _SYM);
                        
                        } else {

                            auto prev_win_rate = rates.find(look_pool -> pool_num - 3);
                            new_lept_for_sale = bal -> lept_for_sale * prev_win_rate -> sell_rate / prev_win_rate -> buy_rate;
                            new_reduced_lepts = new_lept_for_sale * rate -> sell_rate / rate -> buy_rate;
                        
                            forecasts = calculate_forecast(username, host, new_reduced_lepts, look_pool -> pool_num - 1);
                            available = asset(new_lept_for_sale * rate -> sell_rate / LEPTS_PRECISION, _SYM);                        
                        
                        }
                       
                        balance.modify(bal, username, [&](auto &b){
                            b.last_recalculated_win_pool_id = i;
                            b.lept_for_sale = new_lept_for_sale;
                            b.available = available;
                            b.win = true;
                            b.forecasts = forecasts;
                        });

                    } else {

                        spiral_index spiral(_self, host);
                        auto sp = spiral.find(0);
                        balance.modify(bal, username, [&](auto &b){
                            b.last_recalculated_win_pool_id = i;
                            b.win = false;
                            b.available = asset(purchase_amount.amount * (PERCENT_PRECISION - sp -> loss_percent) / PERCENT_PRECISION, _SYM) ;
                        });

                    };
                }
            }

    }


    std::vector <eosio::asset> calculate_forecast(account_name username, account_name host, uint64_t lepts, uint64_t pool_num){
        
        balance_index balance (_self, username);
        rate_index rates (_self, host);
        spiral_index spiral (_self, host);
        auto sp = spiral.find(0);

        std::vector<eosio::asset> forecasts;
        uint64_t lepts_forecast1;
        uint64_t lepts_forecast2;
        uint64_t lepts_forecast3;
        uint64_t lepts_forecast4;

        
        if (pool_num + 3 < sp->pool_limit){
            
            auto rate1 = rates.find(pool_num + 2);
            eosio::asset forecast1 = asset(lepts * rate1 -> sell_rate / LEPTS_PRECISION, _SYM);
            
            forecasts.push_back(forecast1);
            lepts_forecast1 = lepts * rate1 -> sell_rate / rate1 -> buy_rate;

        } 

        if (pool_num + 5 < sp->pool_limit){
           
           auto rate2 = rates.find(pool_num + 4);
           eosio::asset forecast2 = asset(lepts_forecast1 * rate2 -> sell_rate / LEPTS_PRECISION, _SYM);
           lepts_forecast2 = lepts_forecast1 * rate2 -> sell_rate / rate2 -> buy_rate;
           forecasts.push_back(forecast2);
        
        } 

        if (pool_num + 7 < sp->pool_limit){
            auto rate3 = rates.find(pool_num + 6);
            eosio::asset forecast3 = asset(lepts_forecast2 * rate3 -> sell_rate / LEPTS_PRECISION, _SYM);
            lepts_forecast3 = lepts_forecast2 * rate3 -> sell_rate / rate3 -> buy_rate;
           
            forecasts.push_back(forecast3);

        }
        
        if (pool_num + 9 < sp->pool_limit){
            auto rate4 = rates.find(pool_num + 8);
            eosio::asset forecast4 = asset(lepts_forecast3 * rate4 -> sell_rate / LEPTS_PRECISION, _SYM);
            forecasts.push_back(forecast4);
        }
        
        return forecasts;
        

    };


    void syswithdraw_action ( const syswithdraw &op){
        require_auth(op.host);

        auto host = op.host;
        auto sys_balance_id = op.sbalanceid;
        sincome_index sincomes(_self, host);
        dprop_index dprops(_self, host);

        fee_index fee(_self,_self);
        auto f = fee.find(0);
        auto system_fee = f->systemfee;


        auto dprop = dprops.find(0);

        auto sinc = sincomes.find(sys_balance_id);
        eosio_assert(sinc -> withdrawed == false, "System balance is already withdrawed");
        eosio_assert(sinc -> id < dprop -> current_cycle_num - 1, "Only closed cycles can be withdrawed");
        
        eosio::asset total_available = sinc -> available;

        eosio::asset system_amount = asset(system_fee * total_available.amount / PERCENT_PRECISION, _SYM);
        eosio::asset host_amount = total_available - system_amount;
 
        action(
            permission_level{ _self, N(active) },
            N(eosio.token), N(transfer),
            std::make_tuple( _self, host, host_amount, std::string("System Withdraw")) 
        ).send();

        if (system_amount.amount != 0)
            action(
                    permission_level{ _self, N(active) },
                    N(eosio.token), N(transfer),
                    std::make_tuple( _self, _dacomfee, system_amount, std::string("System Withdraw")) 
            ).send();

        sincomes.modify(sinc, host,[&](auto &s){
            s.withdrawed = true;
        });
    }    



    void withdraw_action ( const withdraw &op){
        require_auth(op.username);
        
        auto username = op.username;
        auto balance_id = op.balance_id;
        auto host = op.host;

        refresh_state(username, host);
        
        refreshbal rbalop;
        rbalop.username = op.username;
        rbalop.balance_id = op.balance_id;
        refresh_balance_action(rbalop);

        rate_index rates(_self, host);
        pool_index pools(_self, host);
        dprop_index dprops(_self, host);
        cycle_index cycles(_self, host);
        balance_index balance (_self, username);
        spiral_index spiral(_self, host);

        std::vector<eosio::asset> forecasts;
        
        auto sp = spiral.find(0);
        auto bal = balance.find(balance_id);
        
        eosio_assert(! bal -> withdrawed, "Balance is already withdrawed");

        auto dprop = dprops.find(0);
        auto pool = pools.find(bal->global_pool_id);
        auto cycle = cycles.find(pool -> cycle_num - 1);
        auto next_cycle = cycles.find(pool -> cycle_num);
        auto has_new_cycle = false;
        if (next_cycle != cycles.end())
            has_new_cycle = true;

        auto last_pool = pools.find(cycle -> finish_at_global_pool_id );
        auto rate = rates.find(last_pool -> pool_num - 1 );
        
        eosio_assert(last_pool -> lepts_precision == LEPTS_PRECISION, "Prevent withdraw. Only BP can restore this balance");
        
        uint64_t pools_in_cycle = cycle -> finish_at_global_pool_id - cycle -> start_at_global_pool_id + 1;

        if (((dprop -> current_pool_num == pool -> pool_num ) && (dprop -> current_cycle_num == pool -> cycle_num)) || \
            ((pool -> pool_num < 3) && (pools_in_cycle < 3)) || (has_new_cycle && (pool->pool_num == last_pool -> pool_num)))

        {

            action(
                permission_level{ _self, N(active) },
                N(eosio.token), N(transfer),
                std::make_tuple( _self, username, bal -> purchase_amount, std::string("Withdraw")) 
            ).send();

            pools.modify(last_pool, username, [&](auto &p){
                p.released_lepts = last_pool -> released_lepts - bal -> lept_for_sale;;
                p.remain_lepts = last_pool-> remain_lepts + bal -> lept_for_sale;;
                p.total_in_box = last_pool -> total_in_box - bal -> purchase_amount;
            }); 

            balance.modify(bal, username, [&](auto &b){
                b.sold_amount = bal -> purchase_amount;
                b.date_of_sale = eosio::time_point_sec(now());
                b.lept_for_sale = 0; 
                b.withdrawed = true;
                b.available = asset(0, _SYM);
                b.forecasts = forecasts;
            });

            
            
        } else  { 
            
            auto amount = bal -> available;

            eosio_assert((amount.amount != 0), "Not possible to withdraw zero amount");
        
            action(
                permission_level{ _self, N(active) },
                N(eosio.token), N(transfer),
                std::make_tuple( _self, username, amount, std::string("Withdraw")) 
            ).send();

            print("withdrawed:", bal->lept_for_sale);
            balance.modify(bal, username, [&](auto &b){
                b.sold_amount = bal -> available;
                b.date_of_sale = eosio::time_point_sec(now());
                b.lept_for_sale = 0;  
                b.withdrawed = true;
                b.available = asset(0, _SYM);
                b.forecasts = forecasts;
            });

            if (bal -> win == true)
                pools.modify(last_pool, username, [&](auto &p){
                    p.total_win_withdraw = last_pool -> total_win_withdraw + amount;
                });
            
            else
                pools.modify(last_pool, username, [&](auto &p){
                    p.total_loss_withdraw = last_pool -> total_loss_withdraw + amount;
                });
        
        }
    };


    void upgrade_action(const upgrade &op){
        require_auth(op.username);
        
        eosio_assert(op.purpose.length() < 1024, "Purpose is a maximum 1024 symbols. Describe the idea shortly.");
        eosio_assert(op.purpose.find("&#") , "Symbols '&#' is prohibited in purpose");
        
        fee_index fee(_self,_self);
        auto f = fee.find(0);
        eosio::asset upgrade_fee = f->createhost;
        
        account_index accounts(_self, _self);
        
        auto itr = accounts.find(op.username);
        eosio_assert(itr == accounts.end(), "Account is already upgraded to Host");

        action(
            permission_level{ op.username, N(active) },
            N(eosio.token), N(transfer),
            std::make_tuple( op.username, _dacomfee, upgrade_fee, std::string("Success Upgrade to DACom HOST")) 
        ).send();

        
        accounts.emplace(op.username, [&](auto &a){
            a.username = op.username;
            a.activated = false;
            a.purpose = op.purpose;
            a.registered_at = eosio::time_point_sec(now());
        });

    }


    void apply( uint64_t /*receiver*/, uint64_t code, uint64_t action ) {

      if (code == _self) {
         
         if (action == N(upgrade)){
            upgrade_action(eosio::unpack_action_data<upgrade>());
         }
         if (action == N(setparams)){
            setparams_action(eosio::unpack_action_data<setparams>());
         }
         if (action == N(start)){
            start_action(eosio::unpack_action_data<start>());
         }
         if (action == N(refreshbal)){
            refresh_balance_action(eosio::unpack_action_data<refreshbal>());
         }
         if (action == N(refreshst)){
            auto op = eosio::unpack_action_data<refreshst>();
            refresh_state(op.username, op.host);
         }
         if (action == N(withdraw)){
            withdraw_action(eosio::unpack_action_data<withdraw>());
         }
         if (action == N(syswithdraw)){
            syswithdraw_action(eosio::unpack_action_data<syswithdraw>());
         }
         if (action == N(setfee)){
            setfee_action(eosio::unpack_action_data<setfee>());
         }
     };

     if (action == N(transfer))
     {
        if (code == N(eosio.token)){
            account_name host;
            auto op = eosio::unpack_action_data<eosio::currency::transfer>();

            if (op.from != _self){
                eosio_assert(op.memo != "", "Rejected. Require HOST in memo field");
                if (op.memo.c_str() != "null"){
                    host = eosio::string_to_name(op.memo.c_str());
                    deposit(op.from, host, op.quantity);
                }
            }
        } else {
            eosio_assert(false, "Rejected. Only native tokens can be used on DACom Protocol");
        }
     } 
   }


   

};

extern "C" {
   
   /// The apply method implements the dispatch of events to this contract
   void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
      impl().apply(receiver, code, action);
   }

}; 

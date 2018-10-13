namespace eosio{
struct core {

void priority_goal_enter(const gpriorenter &op){
    auto username = op.username;
    auto host = op.host;
    
    print("SUCCESS CALL PRIORITY ENTER BY GOAL");
    refresh_state(_self, host);

     pool_index pools(_self, host);
     balance_index balances(_self, username);
     dprop_index dprops(_self, host);
     cycle_index cycles(_self, host);
     rate_index rates(_self, host);


     auto dprop = dprops.find(0);
     
     auto first_pool_rate = rates.find(0);
     auto second_pool_rate = rates.find(1);

     auto cycle = cycles.find(dprop-> current_cycle_num - 2);
     
     eosio_assert(dprop->current_pool_num < 2, "Priority Enter is available only for first 2 rounds in cycle");
     eosio_assert(dprop->priority_flag == true, "Priority Enter is available only when priority_flag is enabled");
    
     auto first_pool = pools.find(cycle->finish_at_global_pool_id + 1);
     auto second_pool = pools.find(cycle->finish_at_global_pool_id + 2);
     
     uint64_t first_pool_lepts = first_pool->total_lepts / 2;
     uint64_t second_pool_lepts = first_pool_lepts;
     
     eosio::asset first_pool_amount = asset(first_pool_lepts * first_pool_rate->buy_rate / LEPTS_PRECISION, _SYM);
     eosio::asset second_pool_amount = asset(second_pool_lepts * second_pool_rate->buy_rate / LEPTS_PRECISION, _SYM);
     eosio::asset amount_for_back = first_pool_amount + second_pool_amount;
     print("first_pool_amount: ", first_pool_amount, " ");
     print("second_pool_amount: ", second_pool_amount, " ");


     fill_pool(username, host, first_pool_lepts, first_pool_amount, dprop-> current_pool_id);
     fill_pool(username, host, second_pool_lepts, second_pool_amount, dprop-> current_pool_id + 1);
       


}

void priority_enter(const priorenter &op){
     auto username = op.username;
     auto host = op.host;
     auto balance_id = op.balance_id;
     
     refresh_state(username, host); 

     pool_index pools(_self, host);
     balance_index balances(_self, username);
     dprop_index dprops(_self, host);
     cycle_index cycles(_self, host);
     rate_index rates(_self, host);


     auto dprop = dprops.find(0);
     
     auto first_pool_rate = rates.find(0);
     auto second_pool_rate = rates.find(1);

     auto cycle = cycles.find(dprop-> current_cycle_num - 2);
     //Проверка на предыдущий цикл. Только из предыдущего цикла можно приоритетно войти в новый.

     eosio_assert(dprop->current_pool_num < 2, "Priority Enter is available only for first 2 rounds in cycle");
     eosio_assert(dprop->priority_flag == true, "Priority Enter is available only when priority_flag is enabled");
    
     auto bal = balances.find(balance_id);
     eosio_assert(bal != balances.end(), "Balance not exist");
     eosio_assert(bal->cycle_num == dprop->current_cycle_num - 1, "Only participants from previous cycle can priority enter");
     eosio_assert(bal->last_recalculated_win_pool_id == cycle->finish_at_global_pool_id, "Impossible priority enter with not refreshed balance");
     eosio_assert(bal->win == 0, "Only lose balances can enter by priority");
     eosio_assert(bal->withdrawed == 0, "Balances withdrawed and cant enter on priority");
     eosio_assert(bal->available != bal->purchase_amount, "Nominal dont have priority");

     auto first_pool = pools.find(cycle->finish_at_global_pool_id + 1);
     auto second_pool = pools.find(cycle->finish_at_global_pool_id + 2);
     
     eosio_assert(first_pool->remain_lepts == second_pool->remain_lepts, "Oops, something going wrong. Priority enter is closed.");
     
     uint64_t first_pool_lepts = bal->next_lept_for_sale / 2;
     uint64_t second_pool_lepts = first_pool_lepts;
     
     eosio::asset first_pool_amount = asset(first_pool_lepts * first_pool_rate->buy_rate / LEPTS_PRECISION, _SYM);
     eosio::asset second_pool_amount = asset(second_pool_lepts * second_pool_rate->buy_rate / LEPTS_PRECISION, _SYM);
     eosio::asset amount_for_back = first_pool_amount + second_pool_amount;
     print("first_pool_amount: ", first_pool_amount, " ");
     print("second_pool_amount: ", second_pool_amount, " ");


     fill_pool(username, host, first_pool_lepts, first_pool_amount, dprop-> current_pool_id);
     fill_pool(username, host, second_pool_lepts, second_pool_amount, dprop-> current_pool_id + 1);
       
     
     if (amount_for_back.amount > 0)
         action(
                permission_level{ _self, N(active) },
                N(eosio.token), N(transfer),
                std::make_tuple( _self, username, amount_for_back, std::string("Sediment amount")) 
        ).send();

        auto last_pool = pools.find(cycle->finish_at_global_pool_id);

        pools.modify(last_pool, 0, [&](auto &p){
             p.total_loss_withdraw = last_pool -> total_loss_withdraw + bal->available;
        });

        std::vector<eosio::asset> forecasts;
        balances.modify(bal, 0, [&](auto &b){
            b.sold_amount = bal -> available;
            b.date_of_sale = eosio::time_point_sec(now());
            b.lept_for_sale = 0; 
            b.withdrawed = true;
            b.available = asset(0, _SYM);
            b.forecasts = forecasts;
        });

}

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

        //Добавляем бизнес-доход предыдущего цикла в таблицу без учета последнего пула.
        if ((pool-> pool_num > 2) && (pool->pool_num < sp->pool_limit - 1)){
            auto prev_rate = rates.find(pool->pool_num - 1);
            auto sinc = sincomes.find(dprop->current_cycle_num - 1);

            sincomes.modify(sinc, 0, [&](auto &s){
                s.pool_num = pool -> pool_num;
                s.available = prev_rate -> system_income;
            });
        }

        cycles.emplace(_self, [&](auto &c){
            c.id = cycles.available_primary_key();
            c.start_at_global_pool_id = dprop->current_pool_id + 1; 
            c.finish_at_global_pool_id = dprop->current_pool_id + 2;    
        });

        dprops.modify(dprop, 0, [&](auto &dp){
            dp.current_pool_id  = dprop->current_pool_id + 1;
            dp.cycle_start_at_id = dp.current_pool_id;
            dp.current_cycle_num = dprop->current_cycle_num + 1;
            dp.current_pool_num  = 1;
            dp.priority_flag = true;
                  
        });
        
        uint64_t available_id = pools.available_primary_key();

        pools.emplace(_self, [&](auto &p){
            p.total_lepts = sp->size_of_pool * LEPTS_PRECISION;
            p.id = available_id;
            p.creserved_lepts = 0;
            p.remain_lepts = sp -> size_of_pool * LEPTS_PRECISION;
            p.lept_cost = asset(rate->buy_rate, _SYM);
            p.cycle_num = pool->cycle_num + 1;
            p.pool_num = 1;
            p.color = p.id % 2 == 0 ? "black" : "white"; 
            p.pool_started_at = eosio::time_point_sec(now());
            p.priority_until = eosio::time_point_sec(now()+ sp->priority_seconds);
            p.pool_expired_at = eosio::time_point_sec (now() + sp->pool_timeout + sp->priority_seconds);
            p.total_win_withdraw = asset(0, _SYM);
            p.total_loss_withdraw = asset(0, _SYM);
        });


        pools.emplace(_self, [&](auto &p){
            p.total_lepts = sp->size_of_pool * LEPTS_PRECISION;
            p.id = available_id + 1;
            p.creserved_lepts = 0;
            p.remain_lepts = sp -> size_of_pool * LEPTS_PRECISION;
            p.lept_cost = asset(next_rate->buy_rate, _SYM);
            p.cycle_num = pool->cycle_num + 1;
            p.pool_num = 2;
            p.color = p.id % 2 == 0 ? "black" : "white"; 
            p.pool_started_at = eosio::time_point_sec(now());
            p.priority_until = eosio::time_point_sec(now()+ sp->priority_seconds);
            p.pool_expired_at = eosio::time_point_sec (now() + sp->pool_timeout + sp->priority_seconds);
            p.total_win_withdraw = asset(0, _SYM);
            p.total_loss_withdraw = asset(0, _SYM);
        });


        
        sincomes.emplace(_self, [&](auto &sinc){
            sinc.id = sincomes.available_primary_key();
            sinc.pool_num = 0;
            sinc.available = asset(0, _SYM);
        });

        action(
            permission_level{ _self, N(active) },
            N(goals), N(next),
            std::make_tuple() 
        ).send();

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
        
        auto sp = spiral.find(0);
        
        uint128_t dreserved_lepts = 0;
        uint64_t reserved_lepts = 0;

        
            
        //Если первые два пула не до конца закрыты, это значит, 
        //не все реинвестировали, и пул добавлять не нужно. 
        print("dprop-> current_pool_num", dprop-> current_pool_num, " ");
            
        if (dprop -> current_pool_num > 1) {
            auto rate = rates.find(dprop-> current_pool_num);
            print("dprop-> current_pool_num", dprop-> current_pool_num, " ");
            //Если это обычный пул, то добавляем по 1\

             cycles.modify(cycle, 0, [&](auto &c ){
                c.finish_at_global_pool_id = cycle -> finish_at_global_pool_id + 1;
            });

            dprops.modify(dprop, 0, [&](auto &dp){
               dp.current_pool_num = pool -> pool_num + 1;
               dp.current_pool_id  = pool -> id + 1;
            });
            print("dprop-> current_pool_num_after_modify", dprop-> current_pool_num, " ");
            
            auto prev_prev_pool = pools.find(dprop -> current_pool_id - 2);
            
            dreserved_lepts = (prev_prev_pool -> total_lepts - prev_prev_pool -> remain_lepts ) / LEPTS_PRECISION  * rate -> sell_rate / rate -> buy_rate  \
                  * LEPTS_PRECISION;

            reserved_lepts = uint64_t(dreserved_lepts);


            
            
            
            if ((pool-> pool_num > 1) && (pool->pool_num < sp->pool_limit )){
                auto sinc = sincomes.find(dprop->current_cycle_num - 1);
            
                sincomes.modify(sinc, 0, [&](auto &s){
                    s.pool_num = pool -> pool_num;
                    s.available = rate -> system_income;
                });
            }
                
            pools.emplace(_self, [&](auto &p){
                p.id = pools.available_primary_key();
                p.total_lepts = sp->size_of_pool * LEPTS_PRECISION;
                p.creserved_lepts = prev_prev_pool -> creserved_lepts;
                p.remain_lepts = p.total_lepts - reserved_lepts;
                p.lept_cost = asset(rate->buy_rate, _SYM);
                p.color = p.id % 2 == 0 ? "black" : "white"; 
                p.cycle_num = pool -> cycle_num;
                p.pool_num = pool -> pool_num + 1;
                p.pool_started_at = eosio::time_point_sec(now());
                p.priority_until = eosio::time_point_sec(0);
                p.pool_expired_at = eosio::time_point_sec (now() + sp->pool_timeout);
                p.total_win_withdraw = asset(0, _SYM);
                p.total_loss_withdraw = asset(0, _SYM);
            });

             
        
        } else {
                auto rate = rates.find(dprop-> current_pool_num);
                print("buyrate:", rate->buy_rate);
                
                dprops.modify(dprop, 0, [&](auto &dp){
                   dp.current_pool_num = pool -> pool_num + 1;
                   dp.current_pool_id  = pool -> id + 1;
                   dp.priority_flag = false;
                });  



        }

        


        
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
        
        accounts.modify(account, 0, [&](auto &a){
            a.activated = true;
        });

        cycle_index cycles(_self, op.host);
        pool_index pools(_self, op.host);
        dprop_index dprops(_self, op.host);
        rate_index rates(_self, op.host);

        auto rate = rates.find(0);
        auto next_rate = rates.find(1);
                
        pools.emplace(_self, [&](auto &p){
            p.id = 0;
            p.total_lepts = sp->size_of_pool * LEPTS_PRECISION;
            p.creserved_lepts = 0;
            p.remain_lepts = p.total_lepts;
            p.lept_cost = asset(rate->buy_rate, _SYM);
            p.total_win_withdraw = asset(0, _SYM);
            p.total_loss_withdraw = asset(0, _SYM);
            p.pool_num = 1;
            p.cycle_num = 1;
            p.pool_started_at = eosio::time_point_sec(now());
            p.priority_until = eosio::time_point_sec(0);
            p.pool_expired_at = eosio::time_point_sec (now() + sp->pool_timeout);
            p.color = "black";
        });

          pools.emplace(_self, [&](auto &p){
                p.id = 1;
                p.total_lepts = sp->size_of_pool * LEPTS_PRECISION;
                p.creserved_lepts = 0;
                p.remain_lepts = p.total_lepts;
                p.lept_cost = asset(next_rate->buy_rate, _SYM);
                p.color = p.id % 2 == 0 ? "black" : "white"; 
                p.cycle_num = 1;
                p.pool_num = 2;
                p.pool_started_at = eosio::time_point_sec(now());
                p.priority_until = eosio::time_point_sec(0);
                p.pool_expired_at = eosio::time_point_sec (now() + sp->pool_timeout);
                p.total_win_withdraw = asset(0, _SYM);
                p.total_loss_withdraw = asset(0, _SYM);
            }); 

        
        dprops.emplace(_self, [&](auto &dp){}); 

        cycles.emplace(_self, [&](auto &c){
            c.id = cycles.available_primary_key();
            c.start_at_global_pool_id = 0;
            c.finish_at_global_pool_id = 1;
        });

        sincome_index sincomes(_self, op.host);

        sincomes.emplace(_self, [&](auto &sinc){
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
            fee.emplace(_self, [&](auto &f){
                f.createhost = op.createhost;
                f.systemfee = op.systemfee;
            });
        } else {
     
           fee.modify(fe, 0, [&](auto &f){
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
        eosio_assert(account->payed == true, "Host is not payed and can not start");

        auto size_of_pool = op.size_of_pool;
        auto overlap = op.overlap;
        auto profit_growth = op.profit_growth;
        auto base_rate = op.base_rate;
        auto loss_percent = op.loss_percent;
        auto pool_limit = op.pool_limit;
        auto pool_timeout = op.pool_timeout;
        auto priority_seconds = op.priority_seconds;

        eosio_assert((overlap > 10000) && (overlap < 20000), "Overlap factor must be greater then 10000 (1.0000) and less then 20000 (2.0000)).");
        eosio_assert(profit_growth <= 20000, "Profit growth factor must be greater or equal 0 (0.0000) and less then 20000 (2.0000)).");
        eosio_assert((loss_percent > 0 ) && ( loss_percent <= 10000), "Loss Percent must be greater then 0 (0%) and less or equal 10000 (100%)");
        eosio_assert((base_rate >= 10) && (base_rate < 1000000000), "Base Rate must be greater or equal 10 and less then 1e9");
        eosio_assert((size_of_pool >= 10) && (size_of_pool < 1000000000), "Size of Pool must be greater or equal 10 and less then 1e9");
        eosio_assert((pool_limit >= 3) && (pool_limit < 1000), "Pool Count must be greater or equal 4 and less or equal 1000");
        //eosio_assert((pool_timeout >= 60) && (pool_timeout < 7884000),"Pool Timeout must be greater or equal then 60 sec and less then 7884000 sec");
        //s.priority_seconds 
        auto sp = spiral.find(0);
        auto rate = rates.find(0);
        
        if (((sp != spiral.end()) && rate != rates.end()) && (account -> activated == false)){
            spiral.erase(sp);
            auto it = rates.begin();
            while (it != rates.end()) {
                it = rates.erase(it);
            };
        };

        spiral.emplace(_self, [&](auto &s){
            s.overlap = overlap;
            s.size_of_pool = size_of_pool;
            s.profit_growth = profit_growth;
            s.base_rate = base_rate;
            s.loss_percent = loss_percent;
            s.pool_limit = pool_limit;
            s.pool_timeout = pool_timeout;
            s.priority_seconds = priority_seconds;
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
                    payment_to_loss[i] = live_balance_for_sale[i-3] * (PERCENT_PRECISION - loss_percent) / PERCENT_PRECISION + live_balance_for_sale[i-1] * (PERCENT_PRECISION - loss_percent) / PERCENT_PRECISION;
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
            rates.emplace(_self, [&](auto &r){
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


    
    void deposit ( account_name username, account_name host, eosio::asset amount ){
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
        //eosio_assert(pool -> remain_lepts <= pool->total_lepts, "Prevented Deposit. System error");

        eosio_assert( dprop-> priority_flag == false, "This is a Priority Time");

        eosio_assert ( pool -> pool_expired_at > eosio::time_point_sec(now()), "Rejected. Pool is Expired. Need manual refresh.");
        
        auto rate = rates.find( pool-> pool_num - 1 );
        eosio_assert(amount.amount % rate -> buy_rate == 0, "You can purchase only whole pieces of LEPT. Specify the amount of the multiple purchase rate.");

        uint64_t lepts = LEPTS_PRECISION * amount.amount / rate -> buy_rate;

        eosio_assert(pool -> remain_lepts >= lepts, "Not enought LEPT in target pool");
        
        fill_pool(username, host, lepts, amount, dprop-> current_pool_id);
    
        refresh_state(username, host);
        print("Successful deposit in the DACom protocol");
    };



    void refresh_state (account_name username, account_name host){
       //TODO как учесть приоритетное время? 

        pool_index pools(_self, host);
        dprop_index dprops(_self, host);
        spiral_index spiral(_self, host);

        auto sp = spiral.find(0);
        auto dprop = dprops.find(0);
        auto pool = pools.find(dprop -> current_pool_id);
        
        if (dprop -> priority_flag == true){
            if (pool->pool_started_at + sp->priority_seconds < eosio::time_point_sec(now())){
                
                dprops.modify(dprop, username, [&](auto &dp){
                    dp.priority_flag = false;
                });

                pools.modify(pool, username, [&](auto &p){
                    p.remain_lepts = pool->remain_lepts / LEPTS_PRECISION * LEPTS_PRECISION;
                });

                auto next_pool = pools.find(dprop->current_pool_id + 1);
                pools.modify(next_pool, username, [&](auto &p ){
                    p.remain_lepts = next_pool->remain_lepts / LEPTS_PRECISION * LEPTS_PRECISION; 
                });
                refresh_state(username, host);

            }
        } else {
            //Если пул истек, или доступных пулов больше нет, или оставшихся лепт больше нет, то новый цикл
            if ((pool -> pool_expired_at < eosio::time_point_sec(now()) || \
                ((pool -> pool_num + 1 == sp->pool_limit) && (pool -> remain_lepts == 0)))){
                    
                start_new_cycle(username, host);
          
            } else if ((pool -> remain_lepts == 0)){
            //Если просто нет лепт - новый пул. 
            //На случай, если приоритетные пулы полностью заполнены с остатком менее 1 LEPT, повторный рефреш. 
                next_pool(username, host);
                refresh_state(username, host);
            }
        }
        
    };


    void fill_pool(account_name username, account_name host, uint64_t lepts, eosio::asset amount, uint64_t filled_pool_id){
        std::vector<eosio::asset> forecasts;
        
        cycle_index cycles(_self, host);
        pool_index pools(_self, host);
        dprop_index dprops(_self, host);
        balance_index balance(_self, username);
        rate_index rates(_self, host);

        auto dprop = dprops.find(0);
        auto pool = pools.find(filled_pool_id);
        auto rate = rates.find(dprop->current_pool_num + 1);
        uint64_t next_lept_for_sale;
        uint64_t remain_lepts;

        if (rate == rates.end())
            next_lept_for_sale = lepts;
        else 
            next_lept_for_sale = lepts * rate->sell_rate / rate->buy_rate;
        
        if (lepts > pool -> remain_lepts){
            uint64_t floor_remain_lepts = LEPTS_PRECISION * pool->remain_lepts / LEPTS_PRECISION + LEPTS_PRECISION;
            eosio_assert(lepts <= floor_remain_lepts, "Oops, something wrong.");
            remain_lepts = floor_remain_lepts - lepts;
        } else {
            remain_lepts = pool -> remain_lepts - lepts;
        }
        
        forecasts = calculate_forecast(username, host, lepts, pool -> pool_num - 1);

        pools.modify(pool, 0, [&](auto &p){
            p.remain_lepts = remain_lepts;
//          p.remain_lepts = LEPTS_PRECISION * remain_lepts / LEPTS_PRECISION;
        });

        balance.emplace(_self, [&](auto &b){
            b.id = balance.available_primary_key();
            b.cycle_num = pool->cycle_num;
            b.pool_num = pool->pool_num;
            b.host = host;
            b.global_pool_id = filled_pool_id; 
            b.pool_color = pool -> color;
            b.lept_for_sale = lepts;
            b.next_lept_for_sale = next_lept_for_sale;
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
                    balance.modify(bal, 0, [&](auto &b){
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
                            
                            if (new_reduced_lepts == 0)
                                new_reduced_lepts = new_lept_for_sale;

                            forecasts = calculate_forecast(username, host, new_reduced_lepts, look_pool -> pool_num - 1);
                            available = asset(bal ->lept_for_sale * rate -> sell_rate / LEPTS_PRECISION, _SYM);
                        
                        } else {

                            auto prev_win_rate = rates.find(look_pool -> pool_num - 3);
                            new_lept_for_sale = bal -> lept_for_sale * prev_win_rate -> sell_rate / prev_win_rate -> buy_rate;
                            new_reduced_lepts = new_lept_for_sale * rate -> sell_rate / rate -> buy_rate;
                            
                            if (new_reduced_lepts == 0)
                                new_reduced_lepts = new_lept_for_sale;
                            
                            forecasts = calculate_forecast(username, host, new_reduced_lepts, look_pool -> pool_num - 1);
                            available = asset(new_lept_for_sale * rate -> sell_rate / LEPTS_PRECISION, _SYM);                        
                        
                        }
                       
                        balance.modify(bal, 0, [&](auto &b){
                            b.last_recalculated_win_pool_id = i;
                            b.lept_for_sale = new_lept_for_sale;
                            b.next_lept_for_sale = new_reduced_lepts;
                            b.available = available;
                            b.win = true;
                            b.forecasts = forecasts;
                        });

                    } else {

                        spiral_index spiral(_self, host);
                        auto sp = spiral.find(0);
                        balance.modify(bal, 0, [&](auto &b){
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
        auto username = op.username;

        sincome_index sincomes(_self, host);
        dprop_index dprops(_self, host);

        fee_index fee(_self,_self);
        auto f = fee.find(0);
        auto system_fee = f->systemfee;
        //TODO устранить системную комиссию в процентах

        auto share_supply = eosio::token(N(eosio.token)).get_supply(eosio::symbol_type(_SHARES).name()).amount;
        auto share_balance = eosio::token(N(eosio.token)).get_balance(username, eosio::symbol_type(_SHARES).name()).amount;
        print("share_supply", share_supply, " ");
        print("share_balance", share_balance, " ");

        auto dprop = dprops.find(0);

        auto sinc = sincomes.find(sys_balance_id);
        eosio_assert(sinc -> withdrawed == false, "System balance is already withdrawed");
        //OPEN IT!
        //eosio_assert(sinc -> id < dprop -> current_cycle_num - 1, "Only closed cycles can be withdrawed");
        
        eosio::asset total_available = sinc -> available;
        
        uint64_t fraction = PERCENT_PRECISION * share_balance / share_supply;
        
        eosio::asset amount = asset(total_available.amount * fraction / PERCENT_PRECISION);
        
        print("total_available", total_available, " ");
        print("fraction", fraction, " ");
        
        print("amount", amount, " ");
       
        action(
            permission_level{ _self, N(active) },
            N(eosio.token), N(transfer),
            std::make_tuple( _self, username, amount, std::string("System Withdraw")) 
        ).send();

//TODO withdrawed sys balances for each user
        // sincomes.modify(sinc, 0,[&](auto &s){
        //     s.withdrawed = true;
        // });
    }    



    void withdraw_action ( const withdraw &op){
        require_auth(op.username);
        
        auto username = op.username;
        auto balance_id = op.balance_id;
        auto host = op.host;

        refresh_state(username, host);
        
        rate_index rates(_self, host);
        pool_index pools(_self, host);
        dprop_index dprops(_self, host);
        cycle_index cycles(_self, host);
        balance_index balance (_self, username);
        spiral_index spiral(_self, host);
        std::vector<eosio::asset> forecasts;
        

        auto sp = spiral.find(0);
        auto bal = balance.find(balance_id);
        auto pool = pools.find(bal->global_pool_id);
        auto cycle = cycles.find(pool -> cycle_num - 1);
        
        eosio_assert(bal->last_recalculated_win_pool_id == cycle->finish_at_global_pool_id, "Cannot withdraw not refreshed balance. Refresh Balance first and try again.");
        
        eosio_assert(! bal -> withdrawed, "Balance is already withdrawed");

        auto dprop = dprops.find(0);
        auto next_cycle = cycles.find(pool -> cycle_num);
        auto has_new_cycle = false;
        if (next_cycle != cycles.end())
            has_new_cycle = true;

        auto last_pool = pools.find(cycle -> finish_at_global_pool_id );
        auto rate = rates.find(last_pool -> pool_num - 1 );
        
        eosio_assert(last_pool -> remain_lepts <= pool->total_lepts, "Prevented withdraw. Only BP can restore this balance");
        
        uint64_t pools_in_cycle = cycle -> finish_at_global_pool_id - cycle -> start_at_global_pool_id + 1;
        
        
        if (((dprop -> current_pool_num == pool -> pool_num ) && (dprop -> current_cycle_num == pool -> cycle_num)) || \
            ((pool -> pool_num < 3) && (pools_in_cycle < 3)) || (has_new_cycle && (pool->pool_num == last_pool -> pool_num)))

        {

            action(
                permission_level{ _self, N(active) },
                N(eosio.token), N(transfer),
                std::make_tuple( _self, username, bal -> purchase_amount, std::string("Withdraw")) 
            ).send();


            if (pool -> pool_num < 3){

                pools.modify(pool, 0, [&](auto &p){
                    p.remain_lepts = std::min(pool-> remain_lepts + bal -> lept_for_sale, pool->total_lepts);
                }); 

            } else {

                pools.modify(last_pool, 0, [&](auto &p){
                    p.remain_lepts = std::min(last_pool-> remain_lepts + bal -> lept_for_sale, last_pool->total_lepts);
                });

            }

            balance.modify(bal, 0, [&](auto &b){
                b.sold_amount = bal -> purchase_amount;
                b.date_of_sale = eosio::time_point_sec(now());
                b.lept_for_sale = 0; 
                b.withdrawed = true;
                b.available = asset(0, _SYM);
                b.forecasts = forecasts;
            });

            //balance.erase(bal);
            
        } else  { 
            
            auto amount = bal -> available;
            
            action(
                permission_level{ _self, N(active) },
                N(eosio.token), N(transfer),
                std::make_tuple( _self, username, amount, std::string("Withdraw")) 
            ).send();

            
            uint64_t lepts_from_reserved;
            if (bal -> win == true){

                pools.modify(last_pool, 0, [&](auto &p){
                    auto converted_lepts = bal->lept_for_sale * rate -> sell_rate / rate -> buy_rate;
                    
                    p.creserved_lepts = (last_pool->creserved_lepts + converted_lepts) % LEPTS_PRECISION;
                    p.total_win_withdraw = last_pool -> total_win_withdraw + amount;
                    p.remain_lepts = std::min(last_pool -> remain_lepts + (last_pool->creserved_lepts + converted_lepts) / LEPTS_PRECISION * LEPTS_PRECISION, last_pool->total_lepts);
                   
                });
            }
            else {
                pools.modify(last_pool, 0, [&](auto &p){
                     p.total_loss_withdraw = last_pool -> total_loss_withdraw + amount;
                });
                
                
            }
                

            balance.modify(bal, 0, [&](auto &b){
                b.sold_amount = bal -> available;
                b.date_of_sale = eosio::time_point_sec(now());
                b.lept_for_sale = 0;  
                b.withdrawed = true;
                b.available = asset(0, _SYM);
                b.forecasts = forecasts;
            });

            //balance.erase(bal);
        }
    };

};

}

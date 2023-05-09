using namespace eosio;


void unicore2::emit(eosio::name host, eosio::asset host_income, eosio::asset max_income){
    account_index accounts(_me, host.value);
    auto acc = accounts.find(host.value);
    auto root_symbol = acc->get_root_symbol();

    eosio::name ahost = acc->get_ahost();
    auto main_host = acc->get_ahost();
    
    eosio::asset total_dac_asset = asset(0, host_income.symbol);
    eosio::asset total_sys_asset = asset(0, host_income.symbol);
    
    if (host_income.amount > 0) {
        
        gpercents_index gpercents(_me, _me.value);
        uint64_t syspercent = gpercents.find("system"_n.value) -> value;

        double total_ref = (double)host_income.amount * (double)(acc->referral_percent) / (double)HUNDR_PERCENT;
        
        double total_ref_min_sys = total_ref * (HUNDR_PERCENT - syspercent) / HUNDR_PERCENT;
        double total_ref_sys = total_ref * syspercent / HUNDR_PERCENT;

        double total_dac = (double)host_income.amount * (double)(acc->dacs_percent) / (double)HUNDR_PERCENT;
        
        double total_dac_min_sys = total_dac * (HUNDR_PERCENT - syspercent) / HUNDR_PERCENT;
        double total_dac_sys = total_dac * syspercent / HUNDR_PERCENT;
    
        double total_sys = total_dac_sys + total_ref_sys;

        total_dac_asset = asset((uint64_t)total_dac_min_sys, root_symbol);
        
        total_sys_asset = asset((uint64_t)total_sys, root_symbol);

        if (total_dac_asset.amount > 0) {
            unicore2::spread_to_dacs(host, total_dac_asset, acc -> root_token_contract);
        };
                   
        if (total_sys_asset.amount > 0) {
            // Выплаты на системный аккаунт сообщества. 
            action(
                permission_level{ _me, "active"_n },
                "eosio"_n, "inprodincome"_n,
                std::make_tuple( acc -> root_token_contract, total_sys_asset) 
            ).send();

        };

    };

}



[[eosio::action]] void unicore2::priorenter(eosio::name username, eosio::name host, uint64_t balance_id){
    
     unicore2::refresh_state(host); 
     
     require_auth(username);

     pool_index pools(_me, host.value);
     balance_index balances(_me, host.value);
     cycle_index cycles(_me, host.value);
     account_index accounts(_me, host.value);


     auto acc = accounts.find(host.value);
     auto main_host = acc->get_ahost();
     auto root_symbol = acc->get_root_symbol();

     spiral_index spiral(_me, main_host.value);
     auto sp = spiral.find(0);
     
     auto bal = balances.find(balance_id);
     eosio::check(bal != balances.end(), "Balance not exist");
     
     auto cycle = cycles.find(bal-> cycle_num - 1);
     
     eosio::check(bal -> host == host, "Impossible to enter with balance from another host");
     eosio::check(bal->cycle_num <= acc->current_cycle_num - 1, "Only participants from previous cycle can priority enter");
     eosio::check(bal->last_recalculated_win_pool_id == cycle->finish_at_global_pool_id, "Impossible priority enter with not refreshed balance");
     eosio::check(bal->win == 0, "Only lose balances can enter by priority");
     
     eosio::check(bal->available <= bal->purchase_amount, "Your balance have profit and you cannot use priority enter with this balance.");
     eosio::check(bal->available.amount > 0, "Cannot priority enter with a zero balance. Please, write to help-center for resolve it");

     tail_index tail(_me, host.value);
     auto end = tail.rbegin();
     
     uint64_t id = 0;

     if (end != tail.rend()) {
        id = end -> id + 1;
     };

     tail.emplace(_me, [&](auto &t) {
        t.id = id;
        t.username = username;
        t.enter_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
        t.amount = bal -> available;
        t.exit_pool_id = bal -> global_pool_id;
     });
     
    auto last_pool = pools.find(cycle->finish_at_global_pool_id);

    
    balances.erase(bal);

}


/**
 * @brief Получение параметров нового цикла. Внутренний метод, используемый для создания записей в базе данных, необходимых для запуска нового цикла.      
 *
 * @param[in]  host       The host - имя аккаунта хоста
 * @param[in]  main_host  The main host - указатель на имя аккаунта второстепенного хоста, содержающего измененные параметры вращения  (если такой есть)
 */
void improve_params_of_new_cycle (eosio::name host, eosio::name main_host){
    account_index accounts(_me, host.value);
    auto acc = accounts.find(host.value);
    auto root_symbol = acc->get_root_symbol();
    
    cycle_index cycles(_me, host.value);
    
    auto last_cycle = cycles.find(acc->current_cycle_num - 1);

    cycles.emplace(_me, [&](auto &c){
        c.id = cycles.available_primary_key();
        c.ahost = main_host;
        c.start_at_global_pool_id = last_cycle->finish_at_global_pool_id + 1; 
        c.finish_at_global_pool_id = last_cycle->finish_at_global_pool_id + 2; 
    });

    accounts.modify(acc, _me, [&](auto &dp){
        dp.current_pool_id  = last_cycle->finish_at_global_pool_id + 1;
        dp.cycle_start_id = dp.current_pool_id;
        dp.current_cycle_num = acc->current_cycle_num + 1;
        dp.current_pool_num  = 1;
        //TODO? 
        // dp.priority_flag = true;       
        // priority until? pause_mode mean
    });
}
 
/**
 * @brief Внутренний метод установки первых пулов нового цикла. 
 *
 * @param[in]  parent_host  The parent host
 * @param[in]  main_host    The main host
 * @param[in]  root_symbol  The root symbol
 */

void emplace_first_pools(eosio::name parent_host, eosio::name main_host, eosio::symbol root_symbol, eosio::time_point_sec start_at){
    
    spiral_index spiral(_me, main_host.value);
    auto sp = spiral.find(0);
    eosio::check(sp != spiral.end(), "Protocol is not found. Set parameters at first");
    
    account_index accounts(_me, parent_host.value);
    auto acc = accounts.find(parent_host.value);
    cycle_index cycles(_me, parent_host.value);
    pool_index pools(_me, parent_host.value);

    rate_index rates(_me, main_host.value);
    
    auto rate = rates.find(0);
    auto next_rate = rates.find(1);
    auto next_next_rate = rates.find(2);

    auto available_id = pools.available_primary_key();

    uint64_t total_quants = sp->size_of_pool * sp -> quants_precision;
    
    pools.emplace(_me, [&](auto &p){
        p.id = available_id;
        p.ahost = main_host;
        p.total_quants = total_quants;
        p.creserved_quants = 0;
        p.remain_quants = total_quants;
        p.quant_cost = asset(rate->buy_rate, root_symbol);

        p.pool_cost = asset(sp->size_of_pool * rate->buy_rate, root_symbol);
        p.filled = asset(0, root_symbol);
        p.remain = p.pool_cost;
        p.filled_percent = 0;
        
        p.pool_num = 1;
        p.cycle_num = acc->current_cycle_num;
        p.pool_started_at = start_at;
        p.priority_until = eosio::time_point_sec(0);
        p.pool_expired_at = eosio::time_point_sec (-1);
        p.color = "white";
    });
    
    

    
    pools.emplace(_me, [&](auto &p){
        p.id = available_id + 1;
        p.ahost = main_host;
        p.total_quants = total_quants;
        p.creserved_quants = 0;
        p.remain_quants =total_quants;
        p.quant_cost = asset(next_rate->buy_rate, root_symbol);

        p.pool_cost = asset(sp->size_of_pool * next_rate->buy_rate, root_symbol);
        p.filled = asset(0, root_symbol);
        p.remain = p.pool_cost;
        p.filled_percent = 0;

        p.color = "black";
        p.cycle_num = acc->current_cycle_num;
        p.pool_num = 2;
        p.pool_started_at = start_at;
        p.priority_until = eosio::time_point_sec(0);
        p.pool_expired_at = eosio::time_point_sec (-1);
    }); 

    
    
    action(
        permission_level{ _me, "active"_n },
        "eosio"_n, "pushupdate"_n,
        std::make_tuple( main_host , acc -> current_cycle_num, available_id, eosio::time_point_sec (-1), eosio::time_point_sec (-1)) 
    ).send();
    
}



/**
 * @brief      Внутрений метод запуска нового цикла. 
 * Вызывается при достижении одного из множества условий. Вызывает расчет показательной статистики цикла и установку новых пулов. Если установлен флаг переключения на дочерний хост, здесь происходит замена основного хоста и снятие флага. Дочерний хост хранит в себе измененные параметры финансового ядра. 

 * @param[in]  host  The host
 */
void start_new_cycle ( eosio::name host ) {
    account_index accounts(_me, host.value);
    cycle_index cycles(_me, host.value);
            
    auto acc = accounts.find(host.value);
    eosio::name main_host = acc->get_ahost();
    eosio::name last_ahost = acc->ahost;
            
    auto root_symbol = acc->get_root_symbol();
    eosio::time_point_sec start_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());

    if (acc->need_switch) {

        main_host = (acc->chosts).back();
        
        auto chosts = acc -> chosts;

        chosts.erase(chosts.begin());

        accounts.modify(acc, _me, [&](auto &a){
            a.need_switch = false;
            a.ahost = main_host;
            a.chosts = chosts;
        });

        improve_params_of_new_cycle(host, main_host);

        emplace_first_pools(host, main_host, root_symbol, start_at);

    } else {
        rate_index rates(_me, main_host.value);
        spiral_index spiral(_me, main_host.value);
        
        auto cycle = cycles.find(acc-> current_cycle_num - 1);
        auto rate = rates.find(0);
        auto next_rate = rates.find(1);
        auto sp = spiral.find(0);
        
        pool_index pools(_me, host.value);
        uint64_t available_id = pools.available_primary_key();

        improve_params_of_new_cycle(host, main_host);
        emplace_first_pools(host, main_host, root_symbol, start_at);

    }
        
    unicore2::refresh_state(host);  

};



/**
 * @brief      Внутренний метод открытия следующего пула
 * Вызывается только при условии полного выкупа всех внутренних учетных единиц предыдущего пула. 
 *
 * @param[in]  host  The host
 */
void unicore2::next_pool( eosio::name host) {
    account_index accounts(_me, host.value);
    
    auto acc = accounts.find(host.value);
    auto main_host = acc->get_ahost();
    
    cycle_index cycles(_me, host.value);
    pool_index pools(_me, host.value);

    rate_index rates(_me, main_host.value);
    spiral_index spiral(_me, main_host.value);
    

    auto root_symbol = acc->get_root_symbol();
    auto pool = pools.find(acc -> current_pool_id);
    auto cycle = cycles.find(acc -> current_cycle_num - 1);
    
    auto sp = spiral.find(0);
    
    uint128_t dreserved_quants = 0;
    uint64_t reserved_quants = 0;

    //Если первые два пула не выкуплены, это значит, 
    //не все участники воспользовались приоритетным входом, и пул добавлять не нужно. 

    if (acc -> current_pool_num > 1) {
        auto ratem1 = rates.find(acc-> current_pool_num - 1);
        auto rate = rates.find(acc-> current_pool_num);
        auto ratep1 = rates.find(acc-> current_pool_num + 1);

        eosio::asset host_income = asset(0, root_symbol);
        eosio::asset system_income = asset(0, root_symbol);

        if (rate -> system_income > ratem1 -> system_income) {
            host_income = rate -> system_income - ratem1 -> system_income;
        }
        
        cycles.modify(cycle, _me, [&](auto &c ){
            c.finish_at_global_pool_id = cycle -> finish_at_global_pool_id + 1;
        });
        
        unicore2::emit(host, host_income, rate -> system_income);
        
        accounts.modify(acc, _me, [&](auto &dp){
           dp.current_pool_num = pool -> pool_num + 1;
           dp.current_pool_id  = pool -> id + 1;
        });

        auto prev_prev_pool = pools.find(acc -> current_pool_id - 2);
        
        // dreserved_quants = (prev_prev_pool -> total_quants - prev_prev_pool -> remain_quants ) / sp -> quants_precision  * rate -> sell_rate / rate -> buy_rate * sp -> quants_precision;

        reserved_quants = sp -> size_of_pool - rate -> live_quants_for_sale;
        
        print("reserved_quants: ", reserved_quants);

        eosio::check(pool -> pool_num + 1 <= sp -> pool_limit, "Please, write to help-center for check why you cannot deposit now");
    
        uint64_t pool_id = pools.available_primary_key();
        eosio::time_point_sec expired_at = eosio::time_point_sec (eosio::current_time_point().sec_since_epoch() + sp->pool_timeout);

        pools.emplace(_me, [&](auto &p) {
            p.id = pool_id;
            p.ahost = main_host;
            p.total_quants = sp -> size_of_pool * sp -> quants_precision;
            p.remain_quants = rate -> live_quants_for_sale;
            p.quant_cost = asset(rate -> buy_rate, root_symbol);
            p.cycle_num = pool -> cycle_num;
            p.pool_num = pool -> pool_num + 1;
            p.pool_started_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
            p.priority_until = eosio::time_point_sec(0);
            p.pool_expired_at = expired_at;
            p.pool_cost = rate -> pool_cost;
            p.remain = rate -> live_balance_for_sale;
            p.filled = rate -> pool_cost - rate -> live_balance_for_sale;
            p.filled_percent = p.filled * HUNDR_PERCENT / p.pool_cost;
        });

        action(
            permission_level{ _me, "active"_n },
            "eosio"_n, "pushupdate"_n,
            std::make_tuple( host , acc -> current_cycle_num, pool_id, expired_at, eosio::time_point_sec(eosio::current_time_point().sec_since_epoch()))//powerstat -> window_closed_at) 
        ).send();

    } else {
        //Если это стартовые пулы, то только смещаем указатель
        auto rate = rates.find(acc-> current_pool_num);
       
        accounts.modify(acc, _me, [&](auto &a){
           a.current_pool_num = pool -> pool_num + 1;
           a.current_pool_id  = pool -> id + 1;
        });


        action(
            permission_level{ _me, "active"_n },
            "eosio"_n, "pushupdate"_n,
            std::make_tuple( host , acc -> current_cycle_num, pool -> id + 1, eosio::time_point_sec (-1), eosio::time_point_sec(eosio::current_time_point().sec_since_epoch()))//powerstat -> window_closed_at) 
        ).send();

    }   
         
};


/**
 * @brief      Публичный метод запуска хоста
 * Метод необходимо вызвать для запуска хоста после установки параметров хоста. Добавляет первый цикл, два пула, переключает демонастративный флаг запуска и создает статистические объекты. Подписывается аккаунтом хоста.  
 * @auth    host | chost
 * @ingroup public_actions
 * @param[in]  host        имя аккаунта текущего хоста
 * @param[in]  chost       имя аккаунта дочерного хоста
   
 */
[[eosio::action]] void unicore2::start(eosio::name host, eosio::name chost) {
    if (host == chost)
        require_auth(host);
    else 
        require_auth(chost);

    auto main_host = host;

    account_index accounts(_me, main_host.value);
    gpercents_index gpercents(_me, _me.value);
    auto syspercent = gpercents.find("system"_n.value);
    eosio::check(syspercent != gpercents.end(), "Contract is not active");

    cycle_index cycles (_me, host.value);

    auto account = accounts.find(main_host.value);

    eosio::check(account != accounts.end(), "Account is not upgraded to Host. Please update your account and try again.");
    eosio::check(account->payed == true, "Host is not payed and can not start");

    auto root_symbol = account -> get_root_symbol();
    eosio::check(account->parameters_setted == true, "Cannot start host without setted parameters");
    eosio::time_point_sec start_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    
    auto chosts = account->chosts;

    if (chosts.begin() == chosts.end()) {
        eosio::check(account->activated == false, "Protocol is already active and cannot be changed now.");
        
        accounts.modify(account, _me, [&](auto &a){
            a.activated = true;
        });

        cycles.emplace(_me, [&](auto &c){
            c.ahost = main_host;
            c.id = cycles.available_primary_key();
            c.start_at_global_pool_id = 0;
            c.finish_at_global_pool_id = 1;
        });

        emplace_first_pools(host, main_host, root_symbol, start_at);
        
        
    } else {
        
        eosio::check(account->parameters_setted == true, "Cant start branch without setted parameters");
        main_host = chosts.back();
        eosio::check(account->ahost != main_host, "Protocol is already active and cannot be changed now.");
        eosio::check(main_host == chost, "Wrong last non-active child host");
        
        accounts.modify(account, _me, [&](auto &a){
            a.need_switch = true;
            a.non_active_chost = false;
        });
    };
}


/**
 * @brief      Публичный метод установки параметров протокола двойной спирали
 *  Вызывается пользователем после базовой операции создания хоста и проведения оплаты. Так же вызывается при установке параметров дочернего хоста. Содержит алгоритм финансового ядра. Производит основные расчеты таблиц курсов и валидирует положительность бизнес-дохода. 
 *  

    
 * @param[in]  op    The operation
 */
[[eosio::action]] void unicore2::setparams(eosio::name host, eosio::name chost, uint64_t size_of_pool,
            uint64_t quants_precision, uint64_t overlap, uint64_t profit_growth, uint64_t base_rate,
            uint64_t loss_percent, uint64_t compensator_percent, uint64_t pool_limit, uint64_t pool_timeout, uint64_t priority_seconds)
{
    if (host == chost)
        require_auth(host);
    else 
        require_auth(chost);

    account_index accounts(_me, host.value);
    auto main_host = host;
    auto account = accounts.find(main_host.value);

    eosio::check(account != accounts.end(), "Account is not upgraded to Host. Please update your account and try again.");
    
    auto root_symbol = account -> get_root_symbol();
    auto chosts = account->chosts;

    if (account->non_active_chost == true) {
        main_host = chosts.back();
        eosio::check(account->ahost != main_host, "Protocol is already active and cannot be changed now.");
        eosio::check(main_host == chost, "Wrong last non-active child host");
        spiral_index last_params (_me, (account->ahost).value);
        auto lp = last_params.find(0);
        eosio::check(lp->size_of_pool <= size_of_pool, "Size of pool cannot be decreased in child host");
    } else {
        eosio::check(account->activated == false, "Protocol is already active and cannot be changed now.");
    }

    rate_index rates(_me, main_host.value);
    spiral_index spiral(_me, main_host.value);
    

    eosio::check((overlap > 10000) && (overlap <= 100000000), "Overlap factor must be greater then 10000 (1.0000) and less then 20000 (2.0000))");
    eosio::check(profit_growth <= 100000000, "Profit growth factor must be greater or equal 0 (0.0000) and less then 1000000 (100.0000)).");
    eosio::check((base_rate >= 100) && (base_rate < 1000000000), "Base Rate must be greater or equal 100 and less then 1e9");
    eosio::check((size_of_pool >= 10) && (size_of_pool <= 1000000000000), "Size of Pool must be greater or equal 10 and less then 1e9");
    eosio::check((pool_limit >= 3) && (pool_limit < 1000), "Pool Count must be greater or equal 4 and less or equal 1000");
    eosio::check((pool_timeout >= 1) && (pool_timeout <= 2000000000),"Pool Timeout must be greater or equal then 1 sec and less then 7884000 sec");
    // eosio::check(compensator_percent <= loss_percent, "Compensator Percent must be greater then 0 (0%) and less or equal 10000 (100%)");
    
    auto sp = spiral.find(0);
    auto rate = rates.find(0);
    
    if (((sp != spiral.end()) && rate != rates.end()) && ((account -> activated == false) || (account->ahost != main_host))){
        spiral.erase(sp);
        auto it = rates.begin();
        while (it != rates.end()) {
            it = rates.erase(it);
        };
    };

    spiral.emplace(_me, [&](auto &s) {
        s.overlap = overlap;
        s.quants_precision = quants_precision;
        s.size_of_pool = size_of_pool;
        s.profit_growth = profit_growth;
        s.base_rate = base_rate;
        // s.loss_percent = loss_percent;
        s.pool_limit = pool_limit;
        s.pool_timeout = pool_timeout;
        s.priority_seconds = priority_seconds;
    });

    
    
    double buy_rate[pool_limit+1];
    double sell_rate[pool_limit+1];
    double convert_rate[pool_limit+1];
    double delta[pool_limit+1];
    double client_income[pool_limit+1];
    double pool_cost[pool_limit+1];
    double total_in_box[pool_limit+1];
    double payment_to_wins[pool_limit+1];
    double payment_to_loss[pool_limit+1];
    double system_income[pool_limit+1];
    double live_balance_for_sale[pool_limit+1];
    double live_quants_for_sale[pool_limit+1];


    /**
     *     Математическое ядро алгоритма курса двойной спирали.
     */

    for (auto i=0; i < pool_limit; i++ ){
         if (i == 0){

            buy_rate[i] = base_rate;
            sell_rate[i] = base_rate;
            delta[i] = 0;
            client_income[i] = 0;
            pool_cost[i] = size_of_pool * buy_rate[i];
            total_in_box[i] = pool_cost[i];
            payment_to_wins[i] = 0;
            payment_to_loss[i] = 0;
            system_income[i] = 0;
            live_balance_for_sale[i] = total_in_box[i];
            live_quants_for_sale[i] = size_of_pool * quants_precision;

        } else if (i > 0){ 

            sell_rate[i] = buy_rate[i-1] * overlap / ONE_PERCENT;
            client_income[i] = uint64_t(sell_rate[i]) - uint64_t(buy_rate[i-1]);
            delta[i] = uint64_t(client_income[i] * profit_growth / ONE_PERCENT + delta[i - 1]);
            buy_rate[i] = uint64_t(sell_rate[i] + delta[i]);
            pool_cost[i] = uint64_t(size_of_pool * buy_rate[i]);
            payment_to_wins[i] = uint64_t(size_of_pool * sell_rate[i]);
            live_balance_for_sale[i] = i > 1 ? uint64_t(size_of_pool * (client_income[i] + delta[i] + delta[i -1])) : uint64_t(size_of_pool * buy_rate[i]);
            system_income[i] = i > 1 ? size_of_pool * delta[i - 1] : 0; 
            total_in_box[i] = total_in_box[i - 1] + live_balance_for_sale[i];
            live_quants_for_sale[i] = i > 1 ? live_balance_for_sale[i] * quants_precision / buy_rate[i] : size_of_pool * quants_precision;
            //TODO add system income diff
        
            // Используем по причине необходимости переработки решения дифференциального уравнения системного дохода. Ограничение значительно уменьшает количество возможных конфигураций. 
            eosio::check(system_income[i] >= system_income[i-1], "System income should not decrease");


            if (i > 2)
                eosio::check((client_income[i-1] > 0), "Try to increase Overlap, Size of Pool or Base Rate");      
        } 
    }

    /**
     * Установка таблиц курсов в область памяти хоста
     */

    for (auto i = 0; i < pool_limit; i++){
        rates.emplace(_me, [&](auto &r){
            r.pool_id = i;
            r.buy_rate = buy_rate[i];
            r.sell_rate = sell_rate[i];
            r.client_income = asset(client_income[i], root_symbol);
            r.delta = asset(delta[i], root_symbol);
            r.quant_buy_rate = asset(buy_rate[i], root_symbol);
            r.quant_sell_rate = asset(sell_rate[i], root_symbol);
            r.quant_convert_rate = asset(0, root_symbol);
            r.pool_cost = asset(pool_cost[i], root_symbol);
            r.payment_to_wins = asset(payment_to_wins[i], root_symbol);
            r.payment_to_loss = asset(payment_to_loss[i], root_symbol);
            r.total_in_box = asset(total_in_box[i], root_symbol);
            r.system_income = asset(system_income[i], root_symbol);
            r.live_balance_for_sale = asset(live_balance_for_sale[i], root_symbol);
            r.live_quants_for_sale = live_quants_for_sale[i];
        });
    }

    accounts.modify(account, _me, [&](auto &a){
        a.parameters_setted = true;
    });

}


void unicore2::deposit( eosio::name username, eosio::name host, eosio::asset amount, eosio::name code, std::string message ){
    
    eosio::check( amount.is_valid(), "Rejected. Invalid quantity" );

    pool_index pools(_me, host.value);

    account_index accounts(_me, host.value);
    auto acc = accounts.find(host.value);
    
    eosio::name main_host = acc->get_ahost();

    eosio::check(acc->root_token_contract == code, "Wrong token contract for this host");
    
    rate_index rates(_me, main_host.value);

    spiral_index spiral(_me, main_host.value);
    auto sp = spiral.find(0);

    auto root_symbol = acc->get_root_symbol();

    eosio::check ( amount.symbol == root_symbol, "Rejected. Invalid symbol for this contract.");
    eosio::check(acc != accounts.end(), "Rejected. Host is not founded.");
    eosio::check(acc -> activated == true, "Rejected. Protocol is not active");


    unicore2::fill_master_pool(host, username, amount);

    // auto pool = pools.find( acc -> current_pool_id );

    // eosio::check(pool -> remain_quants <= pool->total_quants, "System Error");
    

    // if (pool -> pool_num > 2){
    //     eosio::check ( pool -> pool_expired_at >= eosio::time_point_sec(eosio::current_time_point().sec_since_epoch()), "Pool is Expired");
    // } else {
    //     eosio::check( pool -> pool_started_at <= eosio::time_point_sec(eosio::current_time_point().sec_since_epoch()), "Pool is not started yet");
    // };

    // auto rate = rates.find( pool-> pool_num - 1 );
    
    // uint128_t dquants = uint128_t(sp -> quants_precision * (uint128_t)amount.amount / (uint128_t)rate -> buy_rate);
    // uint64_t quants = dquants;
    // eosio::check(pool -> remain_quants >= quants, "Not enought Quants in target pool");
    
    // unicore2::fill_pool(username, host, quants, amount, acc -> current_pool_id);
    
    unicore2::refresh_state(host);
    
};


void unicore2::fill_master_pool(eosio::name host, eosio::name username, eosio::asset amount) {
    account_index accounts(_me, host.value);
    auto acc = accounts.find(host.value);
    
    eosio::name main_host = acc->get_ahost();
    auto root_symbol = acc -> get_root_symbol();

    masterpool_index masterpool(_me, host.value);

    auto master = masterpool.find(acc -> current_cycle_num);

    if (master == masterpool.end()) {

        masterpool.emplace(_me, [&](auto &mp){
            mp.cycle_num = acc -> current_cycle_num;
            mp.current_pool_num = 1;
            mp.filled_balance = asset(0, root_symbol);
            mp.remain_balance = amount;
        });

    } else {

        masterpool.modify(master, _me, [&](auto &mp){
            mp.remain_balance += amount;
        });

    };

    masterbal_index masterbalance(_me, host.value);
    masterbalance.emplace(_me, [&](auto &mb){
        mb.id = masterbalance.available_primary_key();
        mb.username = username;
        mb.amount = amount;
    });
}




/**
 * @brief      Внутренний метод заполнения пула.
 * Вызывается в момент совершения депозита пользователем или на приоритетном входе. Создает баланс пользователю и уменьшает количество квантов в пуле. 
 *
 * @param[in]  username        The username
 * @param[in]  host            The host
 * @param[in]  quants          The quants
 * @param[in]  amount          The amount
 * @param[in]  filled_pool_id  The filled pool identifier
 */


void unicore2::fill_pool(eosio::name host) {
    account_index accounts(_me, host.value);
    
    auto acc = accounts.find(host.value);
    auto main_host = acc -> get_ahost();

    cycle_index cycles(_me, host.value);
    pool_index pools(_me, host.value);
    rate_index rates(_me, main_host.value);

    spiral_index spiral(_me, main_host.value);
    auto sp = spiral.find(0);

    masterbal_index masterbalance(_me, host.value);
    balance_index balance(_me, host.value);
    
    auto root_symbol = acc->get_root_symbol();
    auto pool = pools.find(acc -> current_pool_id);

    if (pool -> remain.amount == 0) {
        // print("on next pool");
        //TODO next_pool
        next_pool(host);

    } else {

        auto mb = masterbalance.begin();

        if (mb != masterbalance.end()) {

            eosio::asset amount = mb -> amount;
            eosio::asset diff = asset(0, root_symbol);

            if (pool -> remain < amount) {
                amount = pool -> remain;
                diff = mb -> amount - pool->remain;
            };
            // print("on calc amount: ", amount);
            
            // print("on calc diff: ", diff);

            auto current_rate = rates.find( pool-> pool_num - 1 );
            
            print("current_rate: ", current_rate -> buy_rate);
            

            uint128_t dquants = uint128_t(sp -> quants_precision * (uint128_t)amount.amount / (uint128_t)current_rate -> buy_rate);
            uint64_t quants = dquants;

            print("quants_for_buy: ", quants);

            eosio::check(pool -> remain_quants >= quants, "Not enought Quants in target pool");
            
            auto next_rate = rates.find(pool -> pool_num);

            
            uint64_t remain_quants;

            eosio::check(quants <= pool -> remain_quants, "Not enought quants in target pool. Deposit is prevented for core safety");
            
            remain_quants = pool -> remain_quants - quants;
            print("remain_quants on pool: ", remain_quants);
            print("pool_filled_before_fill: ", pool -> filled);

            pools.modify(pool, _me, [&](auto &p) {
                p.remain_quants = remain_quants;
                uint128_t filled = (pool -> total_quants / sp -> quants_precision - remain_quants / sp -> quants_precision) * pool->quant_cost.amount;
                p.filled = asset(uint64_t(filled), root_symbol);
                
                print("remain_quants on pool: ", remain_quants);
                
                p.remain = pool -> pool_cost - p.filled;
                
                print("p.filled: ", p.filled);
                print("p.remain: ", p.remain);
                
                p.filled_percent = (pool->total_quants - remain_quants) * HUNDR_PERCENT / pool->total_quants;
                print("p.filled_percent: ", p.filled_percent);

            });

            uint64_t balance_id = get_global_id("balances"_n);
            
            balance.emplace(_me, [&](auto &b){
                b.id = balance_id;
                b.owner = mb -> username;
                b.cycle_num = pool->cycle_num;
                b.pool_num = pool->pool_num;
                b.host = host;
                b.chost = main_host;
                b.global_pool_id = acc -> current_pool_id; 
                b.pool_color = pool -> color;
                b.quants_for_sale = quants;
                b.purchase_amount = amount;
                b.available = amount;
                double root_percent = (double)amount.amount  / (double)b.purchase_amount.amount * (double)HUNDR_PERCENT - (double)HUNDR_PERCENT; 
                b.root_percent = uint64_t(root_percent);
                b.last_recalculated_win_pool_id = pool -> pool_num == 1 ? acc->current_pool_id + 1 : acc->current_pool_id;
                b.ref_amount = asset(0, root_symbol);
                b.dac_amount = asset(0, root_symbol);
                b.sys_amount = asset(0, root_symbol);
            });
            

            masterpool_index masterpool(_me, host.value);
            auto master = masterpool.find(acc -> current_cycle_num);
            
            masterpool.modify(master, _me, [&](auto &mp){
                mp.filled_balance += amount;
                mp.remain_balance -= amount; 
            });

            if (diff.amount > 0) {
                
                masterbalance.modify(mb, _me, [&](auto &m){
                    m.amount = mb -> amount - amount;
                });

            } else {

                masterbalance.erase(mb);

            };
        }
    }
}

void unicore2::refresh_state(eosio::name host) {

    account_index accounts(_me, host.value);
    auto acc = accounts.find(host.value);
    auto root_symbol = acc->get_root_symbol();

    eosio::check(acc != accounts.end(), "Host is not found");
    print("in refresh");
    if (acc -> activated == true) {

        eosio::name main_host = acc->get_ahost();

        pool_index pools(_me, host.value);
        spiral_index spiral(_me, main_host.value);
        auto sp = spiral.find(0);
        auto pool = pools.find(acc -> current_pool_id);
        // Если пул истек, или доступных пулов больше нет, или оставшихся квантов больше нет, то новый цикл
        
        if ((pool -> pool_expired_at < eosio::time_point_sec(eosio::current_time_point().sec_since_epoch()) || \
            ((pool -> pool_num + 1 > sp-> pool_limit) && (pool -> remain_quants < sp -> quants_precision)))) {
            
            start_new_cycle(host);

        } else if ((pool -> remain_quants < sp -> quants_precision)) {
            
            // Если просто нет квантов - новый пул. 
            // На случай, если приоритетные пулы полностью заполнены с остатком менее 1 Quant. 
            unicore2::next_pool(host);
            
        } else {
            print("in else");
            masterpool_index masterpool(_me, host.value);
            auto mp = masterpool.find(acc -> current_cycle_num);
            
            if (mp -> remain_balance.amount > 0) {
                print("on fill pool!");
                unicore2::fill_pool(host);    
                print("after fill pool!");
            };
            
        }

    }
   
}
/**
 * @brief      Публичный метод обновления состояния
 * Проверяет пул на истечение во времени или завершение целого количества ядерных Юнитов. Запускает новый цикл или добавляет новый пул. 
 * 
 * //TODO устранить избыточность
 *
 * @param[in]  host  The host
 */

[[eosio::action]] void unicore2::refreshst(eosio::name username, eosio::name host){
    unicore2::refresh_state(host);
};




/**
 * @brief      Публичный метод обновления баланса
 * Пересчет баланса каждого пользователя происходит по его собственному действию. Обновление баланса приводит к пересчету доступной суммы для вывода. 
 *
 * @param[in]  op    The operation
 */


[[eosio::action]] void unicore2::refreshbal(eosio::name username, eosio::name host, uint64_t balance_id, uint64_t partrefresh){
    eosio::check(has_auth(username) || has_auth("refresher"_n) || has_auth("eosio"_n), "missing required authority");
    
    eosio::name payer;

    if (has_auth(username)) {
        payer = username;
    } else if (has_auth("refresher"_n))
        payer = "refresher"_n;
    else payer = "eosio"_n;

    balance_index balance(_me, host.value);
    auto bal = balance.find(balance_id);
    eosio::check(bal != balance.end(), "Balance is not exist or already withdrawed");
    
    auto chost = bal -> get_ahost();
    auto parent_host = bal -> host;
    account_index accounts(_me, parent_host.value);
    
    auto acc = accounts.find(parent_host.value);

    auto root_symbol = acc->get_root_symbol();
    
    unicore2::refresh_state(parent_host);
    
    cycle_index cycles(_me, parent_host.value);
    rate_index rates(_me, chost.value);
    pool_index pools(_me, parent_host.value);
    spiral_index spiral(_me, chost.value);

    auto sp = spiral.find(0);
    
    spiral2_index spiral2(_me, chost.value);

    auto sp2 = spiral2.find(0);
    

    auto pool_start = pools.find(bal -> global_pool_id);
    auto cycle = cycles.find(pool_start -> cycle_num - 1);
    auto next_cycle = cycles.find(pool_start -> cycle_num);
    auto pools_in_cycle = cycle -> finish_at_global_pool_id - cycle -> start_at_global_pool_id + 1;
    
    auto last_pool = pools.find(cycle -> finish_at_global_pool_id );
    auto has_new_cycle = false;

    if (next_cycle != cycles.end())
        has_new_cycle = true;

    uint64_t ceiling;

    if (( partrefresh != 0 )&&(bal -> last_recalculated_win_pool_id + partrefresh < cycle -> finish_at_global_pool_id)){
        ceiling = bal -> last_recalculated_win_pool_id + partrefresh;

    } else {
        ceiling = cycle -> finish_at_global_pool_id;
    }

    if (bal -> last_recalculated_win_pool_id <= cycle -> finish_at_global_pool_id)
        for (auto i = bal -> last_recalculated_win_pool_id + 1;  i <= ceiling; i++){
            auto look_pool = pools.find(i);
            auto purchase_amount = bal-> purchase_amount;
            auto rate = rates.find(look_pool -> pool_num - 1);
                
            
            if (((acc -> current_pool_num == pool_start -> pool_num ) && (acc -> current_cycle_num == pool_start -> cycle_num)) || \
                ((pool_start -> pool_num < 3) && (pools_in_cycle < 3)) || (has_new_cycle && (pool_start->pool_num == last_pool -> pool_num)))

            {   //NOMINAL
                
                balance.modify(bal, payer, [&](auto &b){
                    b.last_recalculated_win_pool_id = i;
                    b.available = purchase_amount;
                });

            } else {

                eosio::asset available;
                
                uint64_t new_quants_for_sale;
                auto prev_prev_rate = rates.find(look_pool -> pool_num - 3);
                auto prev_rate = rates.find(look_pool -> pool_num - 2);

                gpercents_index gpercents(_me, _me.value);
                
                print("diff pool nums: ", look_pool -> pool_num - pool_start -> pool_num);
                uint64_t distance = look_pool -> pool_num - pool_start -> pool_num;

                if (distance == 1 ) {
                    
                    new_quants_for_sale = bal -> quants_for_sale;
                    
                    available = asset(uint64_t((double)new_quants_for_sale * (double)prev_prev_rate -> sell_rate / (double)sp -> quants_precision), root_symbol);                        
                    print("1: ", available);
                    print("quants: ", bal -> quants_for_sale);
                    print("sell_rate: ", prev_rate -> sell_rate);

                    
                } else if (distance == 2 ) {
                    
                    new_quants_for_sale = bal -> quants_for_sale;
                    
                    available = asset(uint64_t((double)new_quants_for_sale * (double)prev_rate -> sell_rate / (double)sp -> quants_precision), root_symbol);                        
                    print("2: ", available);
                    print("quants: ", bal -> quants_for_sale);
                    print("sell_rate: ", prev_rate -> sell_rate);

                    
                } else {
                    
                    new_quants_for_sale = bal -> quants_for_sale * prev_prev_rate -> sell_rate / prev_prev_rate -> buy_rate;
                    available = asset(uint64_t((double)new_quants_for_sale * (double)prev_rate -> sell_rate / (double)sp -> quants_precision), root_symbol);                        
                
                    print("3: ", available);
                    print("quants: ", new_quants_for_sale);
                    print("sell_rate: ", prev_rate -> sell_rate);
                    

                    
                }

                   
                /**
                Для расчетов выплат реферальных вознаграждений необходимо решить дифференциальное уравнение. 
                 */

                auto start_rate = prev_rate;
                auto finish_rate = rate;
                
                uint64_t ref_quants;
                eosio::asset asset_ref_amount;
                eosio::asset asset_sys_amount;
                eosio::asset asset_dac_amount;
                eosio::asset asset_cfund_amount;
                eosio::asset asset_hfund_amount;

                auto syspercent = gpercents.find("system"_n.value);
                eosio::check(syspercent != gpercents.end(), "Contract is not active");

                
                if (look_pool -> pool_num - bal -> pool_num < 1){
                    ref_quants = bal -> quants_for_sale;
                } else {
                    ref_quants = new_quants_for_sale;
                };
                
                // Здесь необходимо взять в расчеты только те границы, при которых системный доход рос, тогда, в setparams можно снять ограничение на это. 
                if (start_rate -> system_income < finish_rate -> system_income){
                    
                    eosio::asset incr_amount2 = finish_rate -> system_income - start_rate -> system_income;
                    
                    uint64_t total = incr_amount2.amount / 2;
                    
                    double total_ref =   (total * (double)ref_quants / (double)sp -> quants_precision * (double)(acc->referral_percent)) / ((double)HUNDR_PERCENT * (double)sp->size_of_pool);
                    double total_ref_min_sys = total_ref * (HUNDR_PERCENT - syspercent -> value) / HUNDR_PERCENT;
                    double total_ref_sys = total_ref * syspercent -> value / HUNDR_PERCENT;

                    double total_dac =   (total * (double)ref_quants / (double)sp -> quants_precision * (double)(acc->dacs_percent))     / ((double)HUNDR_PERCENT * (double)sp->size_of_pool);
                    double total_dac_min_sys = total_dac * (HUNDR_PERCENT - syspercent -> value) / HUNDR_PERCENT;
                    double total_dac_sys = total_dac * syspercent -> value / HUNDR_PERCENT;
                                    
                    double total_sys = total_ref_sys + total_dac_sys;

                    asset_ref_amount = asset((uint64_t)total_ref_min_sys, root_symbol);
                    asset_dac_amount = asset((uint64_t)total_dac_min_sys, root_symbol);
                    asset_sys_amount = asset((uint64_t)total_sys, root_symbol);
               

                } else {
                    asset_ref_amount = asset(0, root_symbol);
                    asset_dac_amount = asset(0, root_symbol);
                    asset_sys_amount = asset(0, root_symbol);
                }
                

                if ((asset_ref_amount).amount > 0) {
                    unicore2::spread_to_refs(host, username, bal -> ref_amount + asset_ref_amount, available, acc -> root_token_contract);
                };

                balance.modify(bal, payer, [&](auto &b) {
                    b.last_recalculated_win_pool_id = i;
                    b.quants_for_sale = new_quants_for_sale;
                    b.available = available;
                    double root_percent = (double)available.amount / (double)bal -> purchase_amount.amount * (double)HUNDR_PERCENT  - (double)HUNDR_PERCENT; 
                    b.root_percent = (int64_t)root_percent;
                });

            }
        }

}


/**
 * @brief      Публичный метод возврата баланса протоколу. 
 * Вывод средств возможен только для полностью обновленных (актуальных) балансов. Производит обмен Юнитов на управляющий токен и выплачивает на аккаунт пользователя. 
 * 
 * Производит расчет реферальных вознаграждений, генерируемых выплатой, и отправляет их всем партнерам согласно установленной формы.
 * 
 * Производит финансовое распределение между управляющими компаниями и целевым фондом сообщества. 
 * 
 * Каждый последующий пул, который участник проходит в качестве победителя, генеририрует бизнес-доход, который расчитывается исходя из того, что в текущий момент, средств всех проигравших полностью достаточно наа выплаты всем победителям с остатком. Этот остаток, в прапорции Юнитов пользователя и общего количества Юнитов в раунде, позволяет расчитать моментальную выплату, которая может быть изъята из системы при сохранении абсолютного балланса. 
 * 
 * Изымаемая сумма из общего котла управляющих токенов, разделяется на три потока, определяемые двумя параметрами: 
 * - Процент выплат на рефералов. Устанавливается в диапазоне от 0 до 100. Отсекает собой ровно ту часть пирога, которая уходит на выплаты по всем уровням реферальной структуры согласно ее формы. 
 * - Процент выплат на корпоративных управляющих. Устанавливается в диапазоне от 0 до 100. 
 * - Остаток от остатка распределяется в фонд целей сообщества.
 * 
 * Таким образом, коэффициенты позволяют распределять единый системный фонд по окружности с тремя секторами, где каждый сектор есть фонд со своим назначением.  

 * Например, если общий доход от движения баланса пользователя по спирали составляет 100 USD, а коэфициенты распределения Рефералов и Корпоративных Управляющих равняются соответственно по 0.5 (50%), то все рефералы получат по 33$, все управляющие получат по 33$, и еще 33$ попадет в фонд целей сообщества. 1$ останется в качестве комиссии округления на делении у протокола. 
 * 
 * 
 * @param[in]  op    The operation
 */

[[eosio::action]] void unicore2::withdraw(eosio::name username, eosio::name host, uint64_t balance_id){
    eosio::check(has_auth(username) || has_auth(_self), "missing required authority");

    unicore2::refresh_state(host);

    account_index accounts(_me, host.value);
    auto acc = accounts.find(host.value);
    eosio::check(acc != accounts.end(), "host is not found");
    
    auto root_symbol = acc->get_root_symbol();
    balance_index balance(_me, host.value);
    
    auto bal = balance.find(balance_id);
    eosio::check(bal != balance.end(), "Balance is not found");
    auto main_host = bal -> get_ahost();
    
    pool_index pools(_me, host.value);
    rate_index rates(_me, main_host.value);
    cycle_index cycles(_me, host.value);
    spiral_index spiral(_me, main_host.value);
    
    auto sp = spiral.find(0);
    
    auto pool = pools.find(bal->global_pool_id);
    
    auto cycle = cycles.find(pool -> cycle_num - 1);
    
    eosio::check(bal->last_recalculated_win_pool_id == cycle->finish_at_global_pool_id, "Cannot withdraw not refreshed balance. Refresh Balance first and try again.");

    auto next_cycle = cycles.find(pool -> cycle_num);
   
    auto has_new_cycle = false;
    if (next_cycle != cycles.end())
        has_new_cycle = true;

    auto last_pool = pools.find(cycle -> finish_at_global_pool_id );
    
    auto rate = rates.find(last_pool -> pool_num - 1 );
    auto next_rate = rates.find(last_pool -> pool_num );
    auto prev_rate = rates.find(last_pool -> pool_num - 2);
    
    eosio::check(last_pool -> remain_quants <= pool -> total_quants, "Prevented withdraw. Only BP can restore this balance");
    eosio::check(bal -> available.amount > 0, "Cannot withdraw a zero balance. Please, write to help-center for resolve it");

    uint64_t pools_in_cycle = cycle -> finish_at_global_pool_id - cycle -> start_at_global_pool_id + 1;
   
    /**
     * Номинал
     * Номинал выплачивается если выполняется одно из условий: 
     *  - баланс куплен в одном из первых двух пулов, а текущий пул не выше второго;
     *  - баланс куплен в текущем пуле;
     *  - баланс куплен в последнем из возможных пулов;

     * 
     */
    if (((acc -> current_pool_num == pool -> pool_num ) && (acc -> current_cycle_num == pool -> cycle_num)) || \
        ((pool -> pool_num < 3) && (pools_in_cycle < 3)) || (has_new_cycle && (pool->pool_num == last_pool -> pool_num)))

    {   //NOMINAL

        action(
            permission_level{ _me, "active"_n },
            acc->root_token_contract, "transfer"_n,
            std::make_tuple( _me, username, bal -> purchase_amount, std::string("User Withdraw")) 
        ).send();


        if (pool -> pool_num < 3) {

            pools.modify(pool, _me, [&](auto &p) {
                p.remain_quants = pool-> remain_quants + bal -> quants_for_sale; //std::min(pool-> remain_quants + bal -> quants_for_sale, pool->total_quants);      
                uint64_t floated_remain_quants = p.remain_quants / sp -> quants_precision * sp -> quants_precision;
                p.filled = asset(( pool->total_quants - floated_remain_quants) / sp -> quants_precision * pool->quant_cost.amount, root_symbol);
                p.filled_percent = (pool->total_quants - p.remain_quants)  * HUNDR_PERCENT / pool->total_quants;
                p.remain = p.pool_cost - p.filled;                
            }); 
         
        } else {

            pools.modify(last_pool, _me, [&](auto &p){
                p.remain_quants = last_pool-> remain_quants + bal -> quants_for_sale; //std::min(last_pool-> remain_quants + bal -> quants_for_sale, last_pool->total_quants);
                uint64_t floated_remain_quants = p.remain_quants / sp -> quants_precision * sp -> quants_precision;
                p.filled = asset(( last_pool->total_quants - floated_remain_quants) / sp -> quants_precision * last_pool->quant_cost.amount, root_symbol);
                p.filled_percent = (last_pool->total_quants - p.remain_quants) * HUNDR_PERCENT / last_pool->total_quants ;
                p.remain = p.pool_cost - p.filled;
            });
         
        }
        
        
        
    } else  { 
    /**
     * Выигрыш или проигрыш.
     * Расчет производится на основе сравнения текущего цвета пула с цветом пула входа. При совпадении цветов - баланс выиграл. При несовпадении - проиграл. 
     * 
     */
        auto amount = bal -> available;
        bool win = (last_pool -> pool_num - bal -> pool_num) > 1;

        if (amount.amount > 0)
            action(
                permission_level{ _me, "active"_n },
                acc->root_token_contract, "transfer"_n,
                std::make_tuple( _me, username, amount, std::string("User Withdraw")) 
            ).send();
    
        
        if (win == true) {

            auto converted_quants = bal->quants_for_sale * rate -> sell_rate / rate -> buy_rate;
            print("on withdraw win balance: ");
            print(" quants for back: ", converted_quants);
            /**
            * Все кванты победителей должны быть возвращены в пулы. 
            */

            uint64_t remain_quants = std::min(last_pool -> remain_quants + converted_quants, last_pool -> total_quants);
            
            pools.modify(last_pool, _me, [&](auto &p) {
                p.remain_quants = remain_quants;
                p.filled = asset( (last_pool->total_quants / sp -> quants_precision - p.remain_quants / sp -> quants_precision) * last_pool->quant_cost.amount, root_symbol);
                p.filled_percent = (last_pool->total_quants - p.remain_quants) * HUNDR_PERCENT / last_pool->total_quants ;
                p.remain = p.pool_cost - p.filled;   
            });
        } 
    }

    balance.erase(bal);
};

    
    void unicore2::spread_to_funds(eosio::name code, eosio::name host, eosio::asset total, std::string comment) {
        gpercents_index gpercents(_me, _me.value);
        auto syspercent = gpercents.find("system"_n.value);
        eosio::check(syspercent != gpercents.end(), "Contract is not active");

        account_index accounts(_me, host.value);
        auto acc = accounts.find(host.value);
        auto main_host = acc->get_ahost();
        auto root_symbol = acc->get_root_symbol();
        
        eosio::check(root_symbol == total.symbol, "Wrong symbol for distribution");
        eosio::check(code == acc->root_token_contract, "Wrong root token for this host");
    
        eosio::asset total_dac_asset;
        eosio::asset total_cfund_asset;
        eosio::asset total_hfund_asset;
        eosio::asset total_sys_asset;
        

        spiral_index spiral(_me, host.value);
        auto sp = spiral.find(0);

        double total_ref = ((double)total.amount * (double)(acc->referral_percent)) / ((double)HUNDR_PERCENT);
        double total_ref_min_sys = total_ref * (HUNDR_PERCENT - syspercent -> value) / HUNDR_PERCENT;
        double total_ref_sys = total_ref * syspercent -> value / HUNDR_PERCENT;


        double total_dac = (double)total.amount * (double)(acc->dacs_percent) / (double)HUNDR_PERCENT;
        double total_dac_min_sys = total_dac * (HUNDR_PERCENT - syspercent -> value) / HUNDR_PERCENT;
        double total_dac_sys = total_dac * syspercent -> value/ HUNDR_PERCENT;
    

        eosio::asset asset_ref_amount = asset((uint64_t)total_ref_min_sys, root_symbol);

        
        total_dac_asset = asset((uint64_t)total_dac_min_sys, root_symbol);

        
        double total_sys = total_dac_sys + total_ref_sys;
        total_sys_asset = asset((uint64_t)total_sys, root_symbol);


        spreads_index spreads(_me, host.value);
        
        spreads.emplace(_me, [&](auto &s){
            s.id = spreads.available_primary_key();
            s.timestamp = eosio::current_time_point();
            s.comment = comment;
            s.total = total;
            s.ref_amount = asset_ref_amount;
            s.dacs_amount = total_dac_asset;
            s.sys_amount = total_sys_asset;
        });        

        //REFS 
        
        if (total_dac_asset.amount > 0) {
            unicore2::spread_to_dacs(host, total_dac_asset, code);
        }

        
        if (total_sys_asset.amount > 0) {
            // Выплаты на системный аккаунт сообщества. 
            action(
                permission_level{ _me, "active"_n },
                "eosio"_n, "inprodincome"_n,
                std::make_tuple( acc -> root_token_contract, total_sys_asset) 
            ).send();
        };


    }


   




/**
 * @brief Публичный метод сжигания квантов по текущему курсу из числа квантов раунда.
 *
*/

void unicore2::burn_action(eosio::name username, eosio::name host, eosio::asset quantity, eosio::name code, eosio::name status){
    
    account_index accounts(_me, host.value);
    auto acc = accounts.find(host.value);
    
    eosio::check(acc != accounts.end(), "Host is not found");
    
    eosio::name main_host = acc->get_ahost();
    auto root_symbol = acc->get_root_symbol();

    eosio::check(code == acc->root_token_contract, "Wrong root token for this host");
    eosio::check(quantity.symbol == root_symbol, "Wrong root symbol for this host");

    pool_index pools(_me, host.value);
    rate_index rates(_me, host.value);

    spiral_index spiral(_me, host.value);
    auto sp = spiral.find(0);

    // auto cycle = cycles.find(pool -> cycle_num - 1);
    auto pool = pools.find(acc->current_pool_id);
    auto rate = rates.find(acc->current_pool_num - 1);
    auto next_rate = rates.find(acc->current_pool_num);

    auto quant_cost = pool -> quant_cost;

    // eosio::check(quantity.amount >= quant_cost.amount, "Not enought quantity for buy 1 quant");
    // eosio::check(quantity.amount % quant_cost.amount == 0, "You can purchase only whole quant");
    
    eosio::asset ref_amount = quantity * acc -> referral_percent / HUNDR_PERCENT;
    eosio::asset core_amount = quantity - ref_amount; 
    
    if (ref_amount.amount > 0) {
        unicore2::spread_to_refs(host, username, ref_amount, quantity, acc -> root_token_contract);
    };

    uint128_t dquants = (uint128_t)core_amount.amount * (uint128_t)sp->quants_precision / (uint128_t)rate->buy_rate;
    uint64_t quants = dquants;
   

    //TODO make recursive and return summ of buys
    // eosio::check(pool->remain_quants >= quants, "Not enought quants in target pool");

    uint64_t quants_for_buy;

    eosio::asset remain_asset = asset(0, root_symbol);
    uint64_t remain_quants = 0;

    if (pool->remain_quants < quants) {

        quants_for_buy = pool->remain_quants;
        remain_asset = quantity - pool->remain;
        remain_quants = quants - pool->remain_quants;
        
    } else {
        
        quants_for_buy = quants;
    
    };

    
    pools.modify(pool, _me, [&](auto &p) {
        p.remain_quants = pool -> remain_quants - quants_for_buy;
        p.filled = asset(( pool -> total_quants - p.remain_quants) / sp -> quants_precision * pool->quant_cost.amount, root_symbol);
        p.filled_percent = (pool -> total_quants - p.remain_quants) * HUNDR_PERCENT / pool->total_quants;
        p.remain = p.pool_cost - p.filled;
        // p.creserved_quants += quants_for_buy; 
    });
    
    
    if (remain_asset.amount > 0) {
        unicore2::spread_to_dacs(host, remain_asset, acc -> root_token_contract);
    }


    unicore2::refresh_state(host);
    
}



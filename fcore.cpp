namespace eosio{
/*

*/
struct core {

    /**
     * @brief Метод ручного пополнения целевого фонда сообщества. Жетоны, попадающие в целевой фонд сообщества, подлежат распределению на цели с помощью голосования участников по установленным правилам консенсуса.  
     * 
     * Метод может не использоваться, поскольку еще одним источником пополнения целевого фонда сообщества является установленный архитектором сообщества процент от финансового оборота ядра. 

     * Примеры:
     * Выпущен 1 млн жетонов, 90% из которых закреплены в целевом фонде, а 10% распределяются среди участников через прямое расределение любым способом (например, продажей). В этом случае, 90% жетонов должны быть помещены в целевой фонд, что гарантирует эмиссию жетонов на цели сообщества в зависимости от вращения ядра и установленного архитектором процента эмиссии при заранее известном общем количестве жетонов.

     * В случае, когда конфигурацией экономики не предусмотрено использование целевого фонда сообществ, или же когда для его пополнения используется только автоматический режим в зависимости от финансового оборота ядра, метод ручного пополнения может не использоваться. И в то же время он всегда доступен любому участнику сообщества простым переводом средств на аккаунт протокола с указанием суб-кода назначения и имени хоста сообщества.   

     * @param[in]  username  The username - имя пользователя, совершающего поолнение. 
     * @param[in]  host      The host - имя аккаунта хоста
     * @param[in]  amount    The amount - сумма для пополнения
     * @param[in]  code      The code - контракт токена, поступивший на вход.
     */

    void fund_emi_pool ( account_name username, account_name host, eosio::asset amount, account_name code ){
        emission_index emis(_self, host);
        auto emi = emis.find(host);
        eosio_assert(emi != emis.end(), "Emission pool for current host is not found");
        account_index accounts(_self, host);
        auto acc = accounts.find(host);
        eosio_assert(acc != accounts.end(), "Host is not found");

        auto root_symbol = acc->get_root_symbol();

        eosio_assert(acc->root_token_contract == code, "Wrong token contract for this pool.");
        eosio_assert ( amount.symbol == root_symbol, "Rejected. Invalid token for this pool.");
        emis.modify(emi, _self, [&](auto &e){
            e.fund = emi->fund + amount;
        });


    };
    
    /**
     * @brief внутренний метод эмиссии, который вызывается в момент распределения целевого фонда сообщества на цели участников. Вызывается в момент переключения порядкового номера пула на каждый последующий. Эмитирует в балансы активных целей сообщества установленный архитектором процент от свободного финансового потока из заранее накопленного в фонде целей сообщества. 
     * 
     * Фонд целей сообщества пополняется в момент вывода выигрыша каждым победителем или прямым пополнением любым участником. 
     * 
     * Фонд целей сообщества расходуется исходя из текущего финансового оборота в спирали при переключении раунда на каждый следующий. 
     * 
     * Распределение в момент переключения пулов определяется параметром процента эмиссии от живого остатка на продажу, что представляет собой  линию обратной связи от динамики вращения ядра.   
     *  
     * Таким образом, целевой фонд сообщества пополняется и расходуется согласно гибкому набору правил, обеспечивающих циркуляцию. 

     * 
     * @param[in]  host  The host - имя аккаунта хоста
     */

    void emission(account_name host){
        
        emission_index emis(_self, host);
        auto emi = emis.find(host);

        if (emi->percent > 0){
            goals_index goals(_self, host);
            account_index accounts(_self, host);
            auto acc = accounts.find(host);
            auto root_symbol = acc->get_root_symbol();

            account_name ahost = acc->get_ahost();
            
            eosio::asset em = adjust_emission_pool(host);  
            eosio::asset on_emission;

            if (emi->fund <= em){
                on_emission = emi->fund;
            } else {
                on_emission = em;
            }


            if (on_emission.amount > 0){
                    auto on_emit = em;
                    eosio::asset fact_emitted = asset(0, em.symbol);
                    std::vector<uint64_t> list_for_emit;
                    uint64_t devider;


                    auto idx_bv = goals.template get_index<N(total_votes)>();
                    auto i_bv = idx_bv.rbegin();
                    bool need_emit = false;

                    
                    uint64_t gcount = 0;
                    
                    while(i_bv != idx_bv.rend() && gcount != emi->gtop) {
                        if((i_bv -> completed == false) && (i_bv -> validated == true )){
                            list_for_emit.push_back(i_bv->id);
                            gcount++;
                        }
                        i_bv++;
                    }        

                    
                    if (gcount < emi->gtop){
                        devider = gcount;
                    } else {
                        devider = emi->gtop;
                    }
                    
           
                    if (devider > 0){

                        eosio::asset each_goal_amount = asset((on_emission).amount / devider, root_symbol);
                        

                        if ((each_goal_amount.amount > 0)){ 
                            need_emit = true;
                        } else {
                            need_emit = false;
                        };
                        
                        
                        eosio::asset for_emit;
                        eosio::asset non_emitted = asset(0, root_symbol);

                        if (need_emit == true){
                            for (auto id : list_for_emit){
                                auto goal_for_emit = goals.find(id);
                                eosio::asset total_recieved = goal_for_emit->available + goal_for_emit->withdrawed;
                                eosio::asset until_fill =  goal_for_emit->target - total_recieved;
                                
                                if (each_goal_amount > until_fill){
                                        for_emit = until_fill; 
                                        non_emitted = non_emitted + each_goal_amount - until_fill;
                                    } else{
                                        for_emit = each_goal_amount;
                                    };
                        
                                fact_emitted += for_emit;

                                goals.modify(goal_for_emit, _self, [&](auto &g){
                                    g.available = goal_for_emit->available + for_emit;
                                    g.completed = g.available + g.withdrawed >= g.target;
                                });
                            }

                            emis.modify(emi, _self, [&](auto &e){
                                e.fund = emi->fund - fact_emitted;
                            });

                      
                        }

                    }
                    
                    cycle_index cycles (_self, host);

                    auto cycle = cycles.find(acc->current_cycle_num-1);
                    
                    cycles.modify(cycle, _self, [&](auto &c){
                        c.emitted = fact_emitted;
                    });
        
                
            }
        }
    }

/**
 * @brief      Внутренний метод расчета пула эмиссии. Вызывается в момент распределения эмиссии на цели сообщества. Расчет объема эмиссии происходит исходя из параметра life_balance_for_sale завершенного пула, и процента эмиссии, установленного архитектором. Процент эмиссии ограничен от 0 до 1000% относительного живого остатка на продажу в каждом новом пуле. 
 * 
 * @param[in]  hostname  The hostname - имя аккаунта хоста
 *
 * @return     { description_of_the_return_value }
 */

    eosio::asset adjust_emission_pool(account_name hostname){
        account_index hosts(_self, hostname);
        emission_index emis(_self, hostname);
        cycle_index cycles(_self, hostname);
        pool_index pools(_self, hostname);

        auto host = hosts.find(hostname);
        auto ahost = host->get_ahost();

        rate_index rates(_self, ahost);
        
        auto last_cycle = cycles.find(host->current_cycle_num - 1); //2 for emission by cycles
        auto pool = pools.find(last_cycle -> finish_at_global_pool_id);
        auto rate = rates.find(pool->pool_num-1);
        auto em = emis.find(hostname);
        eosio::asset for_emit;

        for_emit = asset((rate->live_balance_for_sale).amount * em->percent / 100 / PERCENT_PRECISION, (rate->live_balance_for_sale).symbol );    
        
        return for_emit;
 };

/**
 * @brief      Метод приоритетного входа в новый цикл. Доступен к использованию только при условии наличия предыдущего цикла, в котором участник имеет проигравший баланс. Позволяет зайти частью своего проигравшего баланса одновременно в два пула - первый и второй нового цикла. Использование приоритетного входа возможно только до истечения времени приоритета, которое начинается в момент запуска цикла и продолжается до истечения таймера приоритета. 
 * 
 * Метод позволяет проигравшим балансам предыдущего цикла перераспределиться в новом цикле развития так, чтобы быть в самом начале вращательного движения и тем самым, гарантировать выигрыш. В случае успеха исполнения метода, пользователь обменивает один свой старый проигравший баланс на два новых противоположных цветов. 
 * 
 * В ходе исполнения метода решается арифметическая задача перераспределения, при которой вычисляется максимально возможная сумма входа для имеющегося баланса одновременно в два первых пула. Остаток от суммы, который невозможно распределить на новые пулы по причине нераздельности минимальной учетной единицы, возвращается пользователю переводом. 
 * 
 * Приоритетный вход спроектирован таким образом, то если все проигравшие участники одновременно воспользуются им, то в точности 50% внутренней учетной единицы для первого и второго пула будет выкуплено. 
 * 
 * TODO возможно расширение приоритетного входа до 100% от внутренней учетной единицы для первых двух пулов, а так же, продолжение приоритетного входа для всех последующих пулов.
 * 
 * 
 * @param[in]  op    The operation
 */
void priority_enter(const priorenter &op){
     auto username = op.username;
     auto host = op.host;
     auto balance_id = op.balance_id;
     
     refresh_state(host); 

     pool_index pools(_self, host);
     balance_index balances(_self, username);
     cycle_index cycles(_self, host);
     rate_index rates(_self, host);
     account_index accounts(_self, host);
     spiral_index spiral(_self, host);


     auto acc = accounts.find(host);
     auto main_host = acc->get_ahost();
     auto root_symbol = acc->get_root_symbol();

     auto sp = spiral.find(0);

     auto first_pool_rate = rates.find(0);
     auto second_pool_rate = rates.find(1);

     auto cycle = cycles.find(acc-> current_cycle_num - 2);
     eosio_assert(acc->sale_is_enabled == false, "Priority enter available only when sale is disabled");
     eosio_assert(acc->current_pool_num < 2, "Priority Enter is available only for first 2 rounds in cycle");
     eosio_assert(acc->priority_flag == true, "Priority Enter is available only when priority_flag is enabled");
    
     auto bal = balances.find(balance_id);
     eosio_assert(bal != balances.end(), "Balance not exist");
     eosio_assert(bal->cycle_num == acc->current_cycle_num - 1, "Only participants from previous cycle can priority enter");
     eosio_assert(bal->last_recalculated_win_pool_id == cycle->finish_at_global_pool_id, "Impossible priority enter with not refreshed balance");
     eosio_assert(bal->win == 0, "Only lose balances can enter by priority");
     eosio_assert(bal->withdrawed == 0, "Balances withdrawed and cant enter on priority");
     eosio_assert(bal->available != bal->purchase_amount, "Nominal dont have priority");

     auto first_pool = pools.find(cycle->finish_at_global_pool_id + 1);
     auto second_pool = pools.find(cycle->finish_at_global_pool_id + 2);
     

     //TODO
     uint64_t first_pool_quants = bal->next_quants_for_sale / 2;
     uint64_t second_pool_quants = first_pool_quants;
     
     eosio::asset first_pool_amount = asset(first_pool_quants * first_pool_rate->buy_rate / sp -> quants_precision, root_symbol);
     eosio::asset second_pool_amount = asset(second_pool_quants * second_pool_rate->buy_rate / sp -> quants_precision, root_symbol);
     eosio_assert(first_pool_amount.amount != 0, "Too small amount in Quants for priority enter. Priority enter is not possible for this balance in this Core version.");
     eosio_assert(second_pool_amount.amount != 0, "Too small amount in Quants for priority enter. Priority enter is not possible for this balance in this Core version.");
     
     eosio::asset total_enter = first_pool_amount + second_pool_amount;
     
     /**
      * Здесь происходит перерасчет исходя из того, какое колество жетонов пользователь может получить при условии равного выкупа Юнитов из первых двух пулов. Если после расчетов останется остаток, который не может быть равномерно распределен между пулами - он возвращается пользователю. 
      */

     if (bal->available >= total_enter){
        eosio::asset amount_for_back = asset(bal->available - total_enter);
         action(
                permission_level{ _self, N(active) },
                acc->root_token_contract, N(transfer),
                std::make_tuple( _self, username, amount_for_back, std::string("Sediment amount")) 
        ).send();

    } else {

        /**
         * Здесь у пользователя есть доступная сумма баланса в токенах, которая меньше чем та, с которой участник может зайти в приоритете "на всю котлету". Необходимо найти минимальную сумму в ядерных Юнитах, которая удовлетворяет условиям баланса и приоритетного входа.
         */
        
        second_pool_quants = (bal->available).amount / 2 * sp -> quants_precision / second_pool_rate->buy_rate;
    
        first_pool_quants = second_pool_quants;
        first_pool_amount = asset(first_pool_quants * first_pool_rate->buy_rate / sp -> quants_precision, root_symbol);
    
        second_pool_amount = asset(second_pool_quants * second_pool_rate->sell_rate / sp -> quants_precision, root_symbol);
        eosio::asset amount_for_back = bal->available - (first_pool_amount + second_pool_amount);
        
        eosio_assert(first_pool_amount.amount != 0, "Too small amount in Quants for priority enter. Priority enter is not possible for this balance in this Core version.");
        eosio_assert(second_pool_amount.amount != 0, "Too small amount in Quants for priority enter. Priority enter is not possible for this balance in this Core version.");
     
        action(
            permission_level{ _self, N(active) },
            acc->root_token_contract, N(transfer),
            std::make_tuple( _self, username, amount_for_back, std::string("Sediment amount")) 
        ).send();
    
    }
     
    
    fill_pool(username, host, first_pool_quants, first_pool_amount, acc-> current_pool_id);
    fill_pool(username, host, second_pool_quants, second_pool_amount, acc-> current_pool_id + 1);

    auto last_pool = pools.find(cycle->finish_at_global_pool_id);

    pools.modify(last_pool, username, [&](auto &p){
         p.total_loss_withdraw = last_pool -> total_loss_withdraw + bal->available;
    });

    std::vector<eosio::asset> forecasts;
    balances.modify(bal, username, [&](auto &b){
        b.quants_for_sale = 0; 
        b.withdrawed = true;
        b.available = asset(0, root_symbol);
        b.forecasts = forecasts;
    });

    balances.erase(bal);

}



void adjust_host_income (account_name parent_host, account_name last_ahost){
    //Функция статистики системного дохода хоста
    //TODO - available -> total ?

    account_index accounts(_self, parent_host);
    auto acc = accounts.find(parent_host);   
    sincome_index sincomes(_self, parent_host);

    pool_index pools(_self, parent_host);
    spiral_index spiral(_self, last_ahost);
    rate_index rates(_self, last_ahost);

    auto sp = spiral.find(0);
    auto pool = pools.find(acc -> current_pool_id);
    
    //Добавляем бизнес-доход предыдущего цикла в таблицу без учета последнего пула.
    if ((pool-> pool_num > 2) && (pool->pool_num < sp->pool_limit - 1)){
        auto prev_rate = rates.find(pool->pool_num - 1);
        auto sinc = sincomes.find(acc->current_cycle_num - 1);

        sincomes.modify(sinc, _self, [&](auto &s){
            s.pool_num = pool -> pool_num;
            s.total = prev_rate -> system_income;
        });
    }    

}

/**
 * @brief Получение параметров нового цикла. Внутренний метод, используемый для создания записей в базе данных, необходимых для запуска нового цикла.      
 *
 * @param[in]  host       The host - имя аккаунта хоста
 * @param[in]  main_host  The main host - указатель на имя аккаунта второстепенного хоста, содержающего измененные параметры вращения  (если такой есть)
 */
void improve_params_of_new_cycle (account_name host, account_name main_host){
    account_index accounts(_self, host);
    auto acc = accounts.find(host);
    auto root_symbol = acc->get_root_symbol();
    
    sincome_index sincomes(_self, host);
    cycle_index cycles(_self, host);
    
    auto last_cycle = cycles.find(acc->current_cycle_num - 1);

    cycles.emplace(_self, [&](auto &c){
        c.id = cycles.available_primary_key();
        c.ahost = main_host;
        c.start_at_global_pool_id = last_cycle->finish_at_global_pool_id + 1; 
        c.finish_at_global_pool_id = last_cycle->finish_at_global_pool_id + 2; 
        c.emitted = asset(0, root_symbol);   
    });

    accounts.modify(acc, _self, [&](auto &dp){
        dp.current_pool_id  = last_cycle->finish_at_global_pool_id + 1;
        dp.cycle_start_id = dp.current_pool_id;
        dp.current_cycle_num = acc->current_cycle_num + 1;
        dp.current_pool_num  = 1;
        dp.priority_flag = true;       
    });
    

    sincomes.emplace(_self, [&](auto &sinc){
        sinc.ahost = main_host;
        sinc.id = sincomes.available_primary_key();
        sinc.pool_num = 0;
        sinc.total = asset(0, root_symbol);
        sinc.paid_to_refs = asset(0, root_symbol);
        sinc.paid_to_dacs = asset(0, root_symbol);
        sinc.paid_to_cfund = asset(0, root_symbol);
        sinc.paid_to_sys = asset(0, root_symbol);
    });
}
 
/**
 * @brief Внутренний метод установки первых пулов нового цикла. 
 *
 * @param[in]  parent_host  The parent host
 * @param[in]  main_host    The main host
 * @param[in]  root_symbol  The root symbol
 */

void emplace_first_pools(account_name parent_host, account_name main_host, eosio::symbol_name root_symbol){
    
    spiral_index spiral(_self, main_host);
    auto sp = spiral.find(0);
    eosio_assert(sp != spiral.end(), "Protocol is not found. Set parameters at first");
    sincome_index sincomes(_self, parent_host);
    
    account_index accounts(_self, parent_host);
    auto acc = accounts.find(parent_host);
    cycle_index cycles(_self, parent_host);
    pool_index pools(_self, parent_host);

    rate_index rates(_self, main_host);
    
    auto rate = rates.find(0);
    auto next_rate = rates.find(1);
    auto available_id = pools.available_primary_key();


    pools.emplace(_self, [&](auto &p){
        p.id = available_id;
        p.ahost = main_host;
        p.total_quants = sp->size_of_pool * sp -> quants_precision;
        p.creserved_quants = 0;
        p.remain_quants = p.total_quants;
        p.quant_cost = asset(rate->buy_rate, root_symbol);
        p.total_win_withdraw = asset(0, root_symbol);
        p.total_loss_withdraw = asset(0, root_symbol);
        p.pool_num = 1;
        p.cycle_num = acc->current_cycle_num;
        p.pool_started_at = eosio::time_point_sec(now());
        p.priority_until = acc->current_cycle_num == 1 ? eosio::time_point_sec(0) : eosio::time_point_sec(now()+ sp->priority_seconds);
        p.pool_expired_at = eosio::time_point_sec (-1);
        p.color = "white";
    });

    pools.emplace(_self, [&](auto &p){
        p.id = available_id + 1;
        p.ahost = main_host;
        p.total_quants = sp->size_of_pool * sp -> quants_precision;
        p.creserved_quants = 0;
        p.remain_quants = p.total_quants;
        p.quant_cost = asset(next_rate->buy_rate, root_symbol);
        p.color = "black";
        p.cycle_num = acc->current_cycle_num;
        p.pool_num = 2;
        p.pool_started_at = eosio::time_point_sec(now());
        p.priority_until = acc->current_cycle_num == 1 ? eosio::time_point_sec(0) : eosio::time_point_sec(now()+ sp->priority_seconds);
        p.pool_expired_at = eosio::time_point_sec (-1);
        p.total_win_withdraw = asset(0, root_symbol);
        p.total_loss_withdraw = asset(0, root_symbol);
    }); 
}



/**
 * @brief      Внутрений метод запуска нового цикла. 
 * Вызывается при достижении одного из множества условий. Вызывает расчет показательной статистики цикла и установку новых пулов. Если установлен флаг переключения на дочерний хост, здесь происходит замена основного хоста и снятие флага. Дочерний хост хранит в себе измененные параметры финансового ядра. 

 * @param[in]  host  The host
 */
void start_new_cycle ( account_name host ) {
    account_index accounts(_self, host);
    sincome_index sincomes(_self, host);
    cycle_index cycles(_self, host);
            
    auto acc = accounts.find(host);
    account_name main_host = acc->get_ahost();
    account_name last_ahost = acc->ahost;
            
    auto root_symbol = acc->get_root_symbol();
        
    if (acc->need_switch) {

        main_host = (acc->chosts).back();
        
        accounts.modify(acc, _self, [&](auto &a){
            a.need_switch = false;
            a.ahost = main_host;
        });

        adjust_host_income(host, last_ahost);
        improve_params_of_new_cycle(host, main_host);

        emplace_first_pools(host, main_host, root_symbol);

    } else {
        rate_index rates(_self, main_host);
        spiral_index spiral(_self, main_host);
        
        auto cycle = cycles.find(acc-> current_cycle_num - 1);
        auto rate = rates.find(0);
        auto next_rate = rates.find(1);
        auto sp = spiral.find(0);
        
        adjust_host_income(host, last_ahost);
        
        pool_index pools(_self, host);
        uint64_t available_id = pools.available_primary_key();
        improve_params_of_new_cycle(host, main_host);
        
        emplace_first_pools(host, main_host, root_symbol);

    }
        
        refresh_state(host);  


    };

/**
 * @brief      Внутренний метод открытия следующего пула
 * Вызывается только при условии полного выкупа всех внутренних учетных единиц предыдущего пула. 
 *
 * @param[in]  host  The host
 */
void next_pool( account_name host){
    account_index accounts(_self, host);
    
    auto acc = accounts.find(host);
    auto main_host = acc->get_ahost();
    
    sincome_index sincomes(_self, host);
    cycle_index cycles(_self, host);
    pool_index pools(_self, host);

    rate_index rates(_self, main_host);
    spiral_index spiral(_self, main_host);
    

    auto root_symbol = acc->get_root_symbol();
    auto pool = pools.find(acc -> current_pool_id);
    auto cycle = cycles.find(acc -> current_cycle_num - 1);
    
    auto sp = spiral.find(0);
    
    uint128_t dreserved_quants = 0;
    uint64_t reserved_quants = 0;

    
    //Если первые два пула не выкуплены, это значит, 
    //не все участники воспользовались приоритетным входом, и пул добавлять не нужно. 

    if (acc -> current_pool_num > 1) {
        auto rate = rates.find(acc-> current_pool_num);
        //Если это обычный пул, то добавляем по 1

         cycles.modify(cycle, _self, [&](auto &c ){
            c.finish_at_global_pool_id = cycle -> finish_at_global_pool_id + 1;
        });

        accounts.modify(acc, _self, [&](auto &dp){
           dp.current_pool_num = pool -> pool_num + 1;
           dp.current_pool_id  = pool -> id + 1;
        });
        
        auto prev_prev_pool = pools.find(acc -> current_pool_id - 2);
        
        dreserved_quants = (prev_prev_pool -> total_quants - prev_prev_pool -> remain_quants ) / sp -> quants_precision  * rate -> sell_rate / rate -> buy_rate  \
              * sp -> quants_precision;

        reserved_quants = uint64_t(dreserved_quants);

        if ((pool-> pool_num > 1) && (pool->pool_num < sp->pool_limit )){
            auto sinc = sincomes.find(acc->current_cycle_num - 1);
        
            sincomes.modify(sinc, _self, [&](auto &s){
                s.pool_num = pool -> pool_num;
                s.total = rate -> system_income;
                s.paid_to_refs = asset(0, root_symbol);
            });
        }


        pools.emplace(_self, [&](auto &p){
            p.id = pools.available_primary_key();
            p.ahost = main_host;
            p.total_quants = sp->size_of_pool * sp -> quants_precision;
            p.creserved_quants = prev_prev_pool -> creserved_quants;
            p.remain_quants = p.total_quants - reserved_quants;
            p.quant_cost = asset(rate->buy_rate, root_symbol);
            p.color = prev_prev_pool -> color; 
            p.cycle_num = pool -> cycle_num;
            p.pool_num = pool -> pool_num + 1;
            p.pool_started_at = eosio::time_point_sec(now());
            p.priority_until = eosio::time_point_sec(0);
            p.pool_expired_at = eosio::time_point_sec (now() + sp->pool_timeout);
            p.total_win_withdraw = asset(0, root_symbol);
            p.total_loss_withdraw = asset(0, root_symbol);
        });

         emission(host);
    
    } else {
        //Если это стартовые пулы, то только смещаем указатель
        auto rate = rates.find(acc-> current_pool_num);
        
        accounts.modify(acc, _self, [&](auto &a){
           a.current_pool_num = pool -> pool_num + 1;
           a.current_pool_id  = pool -> id + 1;
           a.priority_flag = false;     
        });
        emission(host);
    }        
};

void fixs (const fixs &op){
    require_auth(op.host);

    account_index accounts(_self, op.host);
    auto acc = accounts.find(op.host);

    accounts.modify(acc, op.host, [&](auto &p){
        p.sale_shift = 0;
    });

}
/**
 * @brief      Публичный метод запуска хоста
 * Метод необходимо вызвать для запуска хоста после установки параметров хоста. Добавляет первый цикл, два пула, переключает демонастративный флаг запуска и создает статистические объекты. Подписывается аккаунтом хоста.  
 * 
 * @param[in]  op    The operation 
 */
void start_action (const start &op){
    if (op.host == op.chost)
        require_auth(op.host);
    else 
        require_auth(op.chost);

    auto main_host = op.host;

    account_index accounts(_self, main_host);

    
    sincome_index sincomes (_self, main_host);
    cycle_index cycles (_self, op.host);

    auto account = accounts.find(main_host);

    eosio_assert(account != accounts.end(), "Account is not upgraded to Host. Please update your account and try again.");
    eosio_assert(account->payed == true, "Host is not payed and can not start");

    auto root_symbol = account -> get_root_symbol();
    eosio_assert(account->parameters_setted == true, "Cannot start host without setted parameters");
    

    auto chosts = account->chosts;

    if (chosts.begin() == chosts.end()) {

        eosio_assert(account->activated == false, "Protocol is already active and cannot be changed now.");
        
        accounts.modify(account, _self, [&](auto &a){
            a.activated = true;
        });
        
        cycles.emplace(_self, [&](auto &c){
            c.ahost = main_host;
            c.id = cycles.available_primary_key();
            c.start_at_global_pool_id = 0;
            c.finish_at_global_pool_id = 1;
            c.emitted = asset(0, root_symbol);
        });

        sincomes.emplace(_self, [&](auto &sinc){
            sinc.ahost = main_host;
            sinc.id = 0;
            sinc.pool_num = 0;
            sinc.total = asset(0, root_symbol);
            sinc.paid_to_dacs = asset(0, root_symbol);
            sinc.paid_to_cfund = asset(0, root_symbol);
            sinc.paid_to_sys = asset(0, root_symbol);
            sinc.paid_to_refs = asset(0, root_symbol);
        });

        emplace_first_pools(op.host, main_host, root_symbol);
        
        ahosts_index ahosts(_self, _self);
        ahosts.emplace(op.host, [&](auto &a){
            a.username = op.host;
        });


    } else {
        
        eosio_assert(account->parameters_setted == true, "Cant start branch without setted parameters");
        main_host = chosts.back();
        eosio_assert(account->ahost != main_host, "Protocol is already active and cannot be changed now.");
        eosio_assert(main_host == op.chost, "Wrong last non-active child host");
        
        accounts.modify(account, _self, [&](auto &a){
            a.need_switch = true;
            a.non_active_chost = false;
        });
    };
}


void del_action(const del &op){
    require_auth(_registrator);
    user_index refs(_self, _self);
    auto u = refs.find(op.username);
    refs.erase(u);
    userscount_index usercounts(_self, _self);
    auto usercount = usercounts.find(0);
    usercounts.modify(usercount, _self, [&](auto &u){
        u.count = u.count - 1;
    });

}

/**
 * @brief      Регистрация пользователя в системе. 
 * Характеризуется созданием профиля с ссылкой на приглашающий аккаунт. Приглашающий аккаунт используется в качестве связи для вычисления партнерских структур различного профиля.
 * 
 * TODO ввести порядковые номера  
 *
 * @param[in]  op    The operation
 */
void reg_action(const reg &op){
    
    eosio_assert(has_auth(op.username) || has_auth(_registrator), "missing required authority");

    eosio_assert( is_account( op.username ), "User account does not exist");
    
     
    user_index refs(_self, _self);
    auto ref = refs.find(op.username);

    eosio_assert(op.username != op.referer, "You cant set the referer yourself");
    
    if (op.username != _self){
        eosio_assert(op.referer != 0, "Registration without referer is not possible");
        eosio_assert( is_account( op.referer ), "Referer account does not exist");
        auto pref = refs.find(op.referer);
        eosio_assert(pref != refs.end(), "Referer is not registered in the core");    
    } 

    
    userscount_index usercounts(_self, _self);
    auto usercount = usercounts.find(0);
    uint64_t count = 1;


    //TODO check account registration;
    // eosio_assert(ref == refs.end(), "Referer is already setted");
    if (ref == refs.end()){
        if (usercount == usercounts.end()){
            usercounts.emplace(_self, [&](auto &u){
                u.id = 0;
                u.count = count;
                u.subject = "registered";
            });
        } else {
            count =  usercount -> count + 1;
            usercounts.modify(usercount, _self, [&](auto &u){
                u.count = count;
            });
        };

        refs.emplace(_self, [&](auto &r){
            r.id = count;
            r.username = op.username;
            r.referer = op.referer;
            r.meta = op.meta;
        });
    } else {
        require_auth(_registrator); //only registrator can change referer

        refs.modify(ref, _self, [&](auto &r){
            r.referer = op.referer;
        });
    }
};

/**
 * @brief      Метод обновления профиля
 * Операция обновления профиля позволяет изменить мета-данные аккаунта. 
 * @param[in]  op    The operation
 */
void profupdate_action(const profupdate &op){
    require_auth(op.username);
    user_index refs(_self, _self);

    auto ref = refs.find(op.username);
    
    refs.modify(ref, op.username, [&](auto &r){
        r.meta = op.meta;
    });

};


/**
 * @brief      Публичный метод установки параметров протокола двойной спирали
 *  Вызывается пользователем после базовой операции создания хоста и проведения оплаты. Так же вызывается при установке параметров дочернего хоста. Содержит алгоритм финансового ядра. Производит основные расчеты таблиц курсов и валидирует положительность бизнес-дохода. 
 *  

    
 * @param[in]  op    The operation
 */
void setparams_action(const setparams &op){
    if (op.host == op.chost)
        require_auth(op.host);
    else 
        require_auth(op.chost);

    account_index accounts(_self, op.host);
    auto main_host = op.host;
    auto account = accounts.find(main_host);

    eosio_assert(account != accounts.end(), "Account is not upgraded to Host. Please update your account and try again.");
    
    auto root_symbol = account -> get_root_symbol();
    auto chosts = account->chosts;

    if (account->non_active_chost == true) {
        main_host = chosts.back();
        eosio_assert(account->ahost != main_host, "Protocol is already active and cannot be changed now.");
        eosio_assert(main_host == op.chost, "Wrong last non-active child host");
        spiral_index last_params (_self, account->ahost);
        auto lp = last_params.find(0);
        eosio_assert(lp->size_of_pool <= op.size_of_pool, "Size of pool cannot be decreased in child host");
    } else {
        eosio_assert(account->activated == false, "Protocol is already active and cannot be changed now.");
    }

    rate_index rates(_self, main_host);
    spiral_index spiral(_self, main_host);
    
    auto size_of_pool = op.size_of_pool;
    auto overlap = op.overlap;
    auto profit_growth = op.profit_growth;
    double base_rate = op.base_rate;

    auto loss_percent = op.loss_percent;
    auto pool_limit = op.pool_limit;
    auto pool_timeout = op.pool_timeout;
    auto priority_seconds = op.priority_seconds;

    auto quants_precision = op.quants_precision;

    eosio_assert((overlap > 10000) && (overlap < 20000), "Overlap factor must be greater then 10000 (1.0000) and less then 20000 (2.0000)).");
    eosio_assert(profit_growth <= 20000, "Profit growth factor must be greater or equal 0 (0.0000) and less then 20000 (2.0000)).");
    eosio_assert((loss_percent > 0 ) && ( loss_percent <= 1000000), "Loss Percent must be greater then 0 (0%) and less or equal 10000 (100%)");
    eosio_assert((base_rate >= 10) && (base_rate < 1000000000), "Base Rate must be greater or equal 100 and less then 1e9");
    eosio_assert((size_of_pool >= 10) && (size_of_pool <= 1000000000000), "Size of Pool must be greater or equal 10 and less then 1e9");
    eosio_assert((pool_limit >= 3) && (pool_limit < 1000), "Pool Count must be greater or equal 4 and less or equal 1000");
    eosio_assert((pool_timeout >= 1) && (pool_timeout < 7884000),"Pool Timeout must be greater or equal then 1 sec and less then 7884000 sec");
    
    auto sp = spiral.find(0);
    auto rate = rates.find(0);
    
    if (((sp != spiral.end()) && rate != rates.end()) && ((account -> activated == false) || (account->ahost != main_host))){
        spiral.erase(sp);
        auto it = rates.begin();
        while (it != rates.end()) {
            it = rates.erase(it);
        };
    };

    spiral.emplace(_self, [&](auto &s){
        s.overlap = overlap;
        s.quants_precision = quants_precision;
        s.size_of_pool = size_of_pool;
        s.profit_growth = profit_growth;
        s.base_rate = base_rate;
        s.loss_percent = loss_percent;
        s.pool_limit = pool_limit;
        s.pool_timeout = pool_timeout;
        s.priority_seconds = priority_seconds;
    });
    
    double buy_rate[pool_limit];
    double sell_rate[pool_limit];
    double convert_rate[pool_limit];
    double delta[pool_limit];
    double client_income[pool_limit];
    double pool_cost[pool_limit];
    double total_in_box[pool_limit];
    double payment_to_wins[pool_limit];
    double payment_to_loss[pool_limit];
    double system_income[pool_limit];
    double live_balance_for_sale[pool_limit];
    


    /**
     *     Математическое ядро алгоритма курса двойной спирали.
     */

    for (auto i=0; i < pool_limit - 1; i++ ){
         if (i == 0){
            buy_rate[i] = base_rate;
            sell_rate[i] = base_rate;

            delta[i] = 0;
            delta[i + 1] = 0;

            client_income[i] = 0;
            client_income[i + 1] = 0;
            
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
                payment_to_loss[i] = live_balance_for_sale[i-1] * (HUNDR_PERCENT - loss_percent) / HUNDR_PERCENT;
            } else if ( i == 3 ){
                payment_to_loss[i] = live_balance_for_sale[i-3] * (HUNDR_PERCENT - loss_percent) / HUNDR_PERCENT + live_balance_for_sale[i-1] * ( HUNDR_PERCENT - loss_percent) / HUNDR_PERCENT;
            } else if ( i > 3 ){
                payment_to_loss[i] = live_balance_for_sale[i-1] * (HUNDR_PERCENT - loss_percent) / HUNDR_PERCENT + payment_to_loss[i - 2];


                eosio_assert(buy_rate[i] > buy_rate[i-1], "Buy rate should growth. Try with a different parameters");
                
                //необходимо переработать решение дифференциального уравнения реферальных поступлений и ассерт может быть снят
                eosio_assert(payment_to_loss[i] >= payment_to_loss[i-1], "Some overflow. Try with a different parameters");
                
            }
            
            system_income[i] = i > 1 ? total_in_box[i-1] - payment_to_wins[i] - payment_to_loss[i] : 0; 
            total_in_box[i] = i > 1 ? total_in_box[i-1] + live_balance_for_sale[i] : total_in_box[i-1] + pool_cost[i];
            

            // Используем по причине необходимости переработки решения дифференциального уравнения системного дохода. 
            eosio_assert(system_income[i] >= system_income[i-1], "System income should not decrease in temporary puproses.");

            /**
             * Проверка бизнес-модели на положительный баланс. Остатка на балансе в любой момент должно быть достаточно для выплат всем проигравшим и всем победителям. Если это не так - протокол спирали не позволит себя создать. 
             */

            bool positive = total_in_box[i-1] - payment_to_wins[i] <= payment_to_loss[i] ? false : true;
            

            eosio_assert(positive, "The financial model of Protocol is Negative. Try with a different parameters");


            if (i > 2)
                eosio_assert((client_income[i-1] > 0), "Overlap rate is too small for Protocol operation. Try to increase Overlap parameter, Size of Pool or Base Rate");      
        } 
    };

    /**
     * Установка таблиц курсов в область памяти хоста
     */

    for (auto i = 0; i < pool_limit - 1; i++){
        rates.emplace(_self, [&](auto &r){
                r.pool_id = i;
                r.buy_rate = buy_rate[i];
                r.sell_rate = sell_rate[i];
                r.client_income = asset(client_income[i], root_symbol);
                r.delta = asset(delta[i], root_symbol);
                r.quant_buy_rate = asset(buy_rate[i], root_symbol);
                r.quant_sell_rate = asset(sell_rate[i], root_symbol);
                r.pool_cost = asset(pool_cost[i], root_symbol);
                r.payment_to_wins = asset(payment_to_wins[i], root_symbol);
                r.payment_to_loss = asset(payment_to_loss[i], root_symbol);
                r.total_in_box = asset(total_in_box[i], root_symbol);
                r.system_income = asset(system_income[i], root_symbol);
                r.live_balance_for_sale = asset(live_balance_for_sale[i], root_symbol);
            });
    }

    accounts.modify(account, _self, [&](auto &a){
        a.parameters_setted = true;
    });

}



void deposit ( account_name username, account_name host, eosio::asset amount, account_name code ){
    require_auth(username);
    eosio_assert( amount.is_valid(), "Rejected. Invalid quantity" );

    user_index users(_self,_self);
    auto user = users.find(username);
    eosio_assert(user != users.end(), "User is not registered");
    
    if (user == users.end()){
        
        //TODO check account registration;
        users.emplace(_self, [&](auto &r){
            r.username = username;
            r.referer = _registrator;
        });
    }
    account_index accounts(_self, host);
    auto acc = accounts.find(host);

    eosio_assert(acc->root_token_contract == code, "Wrong token contract for this host");

    account_name main_host = acc->get_ahost();
    
    rate_index rates(_self, main_host);
    pool_index pools(_self, host);
    spiral_index spiral(_self, host);
    auto sp = spiral.find(0);

    auto root_symbol = acc->get_root_symbol();

    
    eosio_assert ( amount.symbol == root_symbol, "Rejected. Invalid symbol for this contract.");
    eosio_assert(acc != accounts.end(), "Rejected. Host is not founded.");
    eosio_assert(acc -> activated == true, "Rejected. Protocol is not active");

    auto pool = pools.find( acc -> current_pool_id );

    eosio_assert(pool -> remain_quants <= pool->total_quants, "System Error");
    
    eosio_assert( acc-> priority_flag == false, "This is a Priority Time");

    if (pool -> pool_num > 2){
        eosio_assert ( pool -> pool_expired_at > eosio::time_point_sec(now()), "Pool is Expired");
    };

    auto rate = rates.find( pool-> pool_num - 1 );
    eosio_assert(amount.amount % rate -> buy_rate == 0, "You can purchase only whole Quant");

    uint128_t dquants = uint128_t(sp -> quants_precision * (uint128_t)amount.amount / (uint128_t)rate -> buy_rate);
    uint64_t quants = dquants;
   
    eosio_assert(pool -> remain_quants >= quants, "Not enought Quants in target pool");
            
    fill_pool(username, host, quants, amount, acc -> current_pool_id);
    
    refresh_state(host);
    
};


/**
 * @brief      Публичный метод обновления состояния
 * Проверяет пул на истечение во времени или завершение целого количества ядерных Юнитов. Запускает новый цикл или добавляет новый пул. 
 * 
 * //TODO устранить избыточность
 *
 * @param[in]  host  The host
 */

void refresh_state (account_name host){

    account_index accounts(_self, host);
    auto acc = accounts.find(host);
    account_name main_host = acc->get_ahost();

    pool_index pools(_self, host);
    spiral_index spiral(_self, main_host);
    auto sp = spiral.find(0);
    auto pool = pools.find(acc -> current_pool_id);
    print("test0");
    if (acc -> priority_flag == true){
        
        if (pool->pool_started_at + sp->priority_seconds < eosio::time_point_sec(now()) ||
            (pool -> remain_quants < sp -> quants_precision)){
            
            print("test1");

            accounts.modify(acc, _self, [&](auto &dp){
                dp.priority_flag = false;
            });
    
            pools.modify(pool, _self, [&](auto &p){
                p.remain_quants = pool->remain_quants / sp -> quants_precision * sp -> quants_precision;
            });

            
            auto next_pool = pools.find(acc->current_pool_id + 1);
            
            pools.modify(next_pool, _self, [&](auto &p ){
                p.remain_quants = next_pool->remain_quants / sp -> quants_precision * sp -> quants_precision; 
            });
            print("test15");
            refresh_state(host);

        }
    } else {
        // Если пул истек, или доступных пулов больше нет, или оставшихся лепт больше нет, то новый цикл
        if ((pool -> pool_expired_at < eosio::time_point_sec(now()) || \
            ((pool -> pool_num + 1 == sp->pool_limit) && (pool -> remain_quants == 0)))){
            print("try to add new cycle");
            start_new_cycle(host);
      
        } else if ((pool -> remain_quants < sp -> quants_precision)){
        // Если просто нет лепт - новый пул. 
        // На случай, если приоритетные пулы полностью заполнены с остатком менее 1 Quant. 
            print("try to add new pool");
            next_pool(host);
            refresh_state(host);

        } else if (acc->current_pool_num < 3) {
            // Отдельно округляем остатки в случае, если приоритетным входом или целями воспользовались только частично

            pools.modify(pool, _self, [&](auto &p ){
                p.remain_quants = pool->remain_quants / sp -> quants_precision * sp -> quants_precision; 
            });

            if (pool-> remain_quants == 0){
                //Режим ожидания для первых двух пулов
                next_pool(host);
            }
        }
    }
    
};


/**
 * @brief      Внутренний метод заполнения пула.
 * Вызывается в момент совершения депозита пользователем или на приоритетном входе. Создает баланс пользователю и уменьшает количество Юнитов в пуле. 
 *
 * @param[in]  username        The username
 * @param[in]  host            The host
 * @param[in]  quants          The quants
 * @param[in]  amount          The amount
 * @param[in]  filled_pool_id  The filled pool identifier
 */


void fill_pool(account_name username, account_name host, uint64_t quants, eosio::asset amount, uint64_t filled_pool_id){
    std::vector<eosio::asset> forecasts;
    account_index accounts(_self, host);
    
    auto acc = accounts.find(host);
    auto main_host = acc -> get_ahost();

    cycle_index cycles(_self, host);
    pool_index pools(_self, host);
    rate_index rates(_self, main_host);
    spiral_index spiral(_self, host);
    auto sp = spiral.find(0);
    balance_index balance(_self, username);
    
    auto root_symbol = acc->get_root_symbol();
    auto pool = pools.find(filled_pool_id);
    eosio_assert(pool != pools.end(), "Cant FIND");
    print("pool_num + 1", acc->current_pool_num + 1);
    auto rate = rates.find(acc->current_pool_num + 1);
    auto ratem1 = rates.find(acc->current_pool_num);

    uint64_t next_quants_for_sale;
    uint64_t remain_quants;

    if (rate == rates.end())
        next_quants_for_sale = quants;    
    else 
        next_quants_for_sale = quants * rate->sell_rate / rate->buy_rate;
    
    uint64_t b_id = 0;

    eosio_assert(quants <= pool -> remain_quants, "Not enought quants in target pool. Deposit is prevented for core safety.");

    remain_quants = pool -> remain_quants - quants;
    
    forecasts = calculate_forecast(username, host, quants, pool -> pool_num - 1, amount);
    
    pools.modify(pool, _self, [&](auto &p){
        p.remain_quants = remain_quants;
        //p.remain_quants = sp -> quants_precision * remain_quants / sp -> quants_precision;
    });

    b_id = balance.available_primary_key();
    uint64_t amount_in_units = 0;
    
    if (acc-> sale_is_enabled){
        uint64_t sell_rate;
        if (rate != rates.end()){

            sell_rate = rate -> sell_rate;
        
        } else {
        
            sell_rate = ratem1 -> sell_rate;
        
        }
    
        amount_in_units = (double)quants / (double)sp->quants_precision * (double)sell_rate / (double)sp->base_rate * pow(10, acc -> sale_shift);
    } 

    balance.emplace(_self, [&](auto &b){
        b.id = b_id;
        b.cycle_num = pool->cycle_num;
        b.pool_num = pool->pool_num;
        b.host = host;
        b.chost = main_host;
        b.global_pool_id = filled_pool_id; 
        b.pool_color = pool -> color;
        b.quants_for_sale = quants;
        b.next_quants_for_sale = next_quants_for_sale;
        b.purchase_amount = amount;
        b.available = amount;
        b.if_convert = asset(amount_in_units, (acc->asset_on_sale).symbol);
        b.last_recalculated_win_pool_id = pool -> id;
        b.forecasts = forecasts;
        b.ref_amount = asset(0, root_symbol);
        b.dac_amount = asset(0, root_symbol);
        b.cfund_amount = asset(0, root_symbol);
        b.sys_amount = asset(0, root_symbol);
    });
    print("test3, ");
}

/**
 * @brief      Публичный метод обновления баланса
 * Пересчет баланса каждого пользователя происходит по его собственному действию. Обновление баланса приводит к пересчету доступной суммы для вывода - на противоположном цвете - проигрыш, на своем - выигрыш. На общее состояние Юнитов в пуле действие или бездействие пользователя со своим балансом влияния не оказывает. Без обновления баланса до состояния крайнего пула - сдать баланс Юнитов протоколу невозможно. 
 *
 * @param[in]  op    The operation
 */


void refresh_balance_action (const refreshbal &op){
    require_auth(op.username);
    account_name username = op.username;
    uint64_t balance_id = op.balance_id;


    balance_index balance (_self, username);
    auto bal = balance.find(balance_id);
    eosio_assert(bal != balance.end(), "Balance is not exist or already withdrawed");
    
    auto chost = bal -> get_ahost();
    auto parent_host = bal -> host;
    account_index accounts(_self, parent_host);
    
    auto acc = accounts.find(parent_host);

    auto root_symbol = acc->get_root_symbol();
    
    refresh_state(parent_host);
    
    cycle_index cycles(_self, parent_host);
    rate_index rates(_self, chost);
    pool_index pools(_self, parent_host);
    spiral_index spiral(_self, chost);

    auto sp = spiral.find(0);
    
    auto pool_start = pools.find(bal -> global_pool_id);
    auto cycle = cycles.find(pool_start -> cycle_num - 1);
    auto next_cycle = cycles.find(pool_start -> cycle_num);
    auto pools_in_cycle = cycle -> finish_at_global_pool_id - cycle -> start_at_global_pool_id + 1;
    
    auto last_pool = pools.find(cycle -> finish_at_global_pool_id );
    auto has_new_cycle = false;

    if (next_cycle != cycles.end())
        has_new_cycle = true;

    uint64_t ceiling;

    if (( op.partrefresh != 0 )&&(bal -> last_recalculated_win_pool_id + op.partrefresh < cycle -> finish_at_global_pool_id)){
        ceiling = bal -> last_recalculated_win_pool_id + op.partrefresh;

    } else {
        ceiling = cycle -> finish_at_global_pool_id;
    }


    if (bal -> last_recalculated_win_pool_id <= cycle -> finish_at_global_pool_id)
        for (auto i = bal -> last_recalculated_win_pool_id + 1;  i <= ceiling; i++){
            std::vector<eosio::asset> forecasts;
            std::vector<eosio::asset> forecasts2;

            auto look_pool = pools.find(i);
            auto purchase_amount = bal-> purchase_amount;
            auto rate = rates.find(look_pool -> pool_num - 1);
                

            eosio::asset if_convert;

            if (acc -> sale_is_enabled == true){    
                auto units_amount_on_exchange = (double)bal -> quants_for_sale * (double)rate->sell_rate / (double)sp->base_rate  / (double)sp->quants_precision * pow(10, acc -> sale_shift); 
                if_convert = asset(units_amount_on_exchange, (acc->asset_on_sale).symbol);
            } else {
                if_convert = asset(0, root_symbol);
            }


            if (((acc -> current_pool_num == pool_start -> pool_num ) && (acc -> current_cycle_num == pool_start -> cycle_num)) || \
                ((pool_start -> pool_num < 3) && (pools_in_cycle < 3)) || (has_new_cycle && (pool_start->pool_num == last_pool -> pool_num)))

            {
                print("test");
                balance.modify(bal, username, [&](auto &b){
                  b.last_recalculated_win_pool_id = i;
                  b.available = purchase_amount;
                  b.if_convert = if_convert;
                });

            } else {
                
                eosio::asset available;
                uint64_t new_reduced_quants;
                uint64_t new_quants_for_sale;
                eosio::asset ref_amount = asset(0, root_symbol);
                auto prev_win_rate = rates.find(look_pool -> pool_num - 3);
                auto middle_rate = rates.find(look_pool -> pool_num - 2);

                if (pool_start -> color == look_pool -> color){
                    //WIN
                    if (look_pool -> pool_num - pool_start -> pool_num <= 2){
                        
                        new_reduced_quants = bal -> quants_for_sale * rate -> sell_rate / rate -> buy_rate;
                        new_quants_for_sale = bal -> quants_for_sale;
                        
                        if (new_reduced_quants == 0)
                            new_reduced_quants = new_quants_for_sale;

                        forecasts = calculate_forecast(username, parent_host, new_quants_for_sale, look_pool -> pool_num - 3, bal->purchase_amount);
                        available = asset(new_quants_for_sale * rate -> sell_rate / sp -> quants_precision, root_symbol);                        
                    
                    } else {

                        new_quants_for_sale = bal -> quants_for_sale * prev_win_rate -> sell_rate / prev_win_rate -> buy_rate;
                        new_reduced_quants = new_quants_for_sale * rate -> sell_rate / rate -> buy_rate;
                        
                        if (new_reduced_quants == 0)
                            new_reduced_quants = new_quants_for_sale;
                        print("new_reduced_quants", new_reduced_quants);
                        
                        forecasts = calculate_forecast(username, parent_host, new_quants_for_sale, look_pool -> pool_num - 3, bal->purchase_amount);
                        available = asset(new_quants_for_sale * rate -> sell_rate / sp -> quants_precision, root_symbol);                        
                    
                    }

                       
                    /**
                    Для расчетов выплат реферальных вознаграждений необходимо решить дифференциальное уравнение. 
                     */

                    auto start_rate = prev_win_rate;
                    auto finish_rate = rate;
                    // print("start_rate: ", start_rate->system_income, " ");
                    // print("finish_rate: ", finish_rate->system_income, " ");
                    
                    uint64_t ref_quants;
                    eosio::asset asset_ref_amount;
                    eosio::asset asset_sys_amount;
                    eosio::asset asset_dac_amount;
                    eosio::asset asset_cfund_amount;

                    if (bal->pool_num == start_rate -> pool_id + 1){
                        ref_quants = bal->quants_for_sale / sp -> quants_precision * sp -> quants_precision;
                    
                    } else {
                        ref_quants = new_quants_for_sale / sp -> quants_precision * sp -> quants_precision;
                    }
                    
                    // Здесь необходимо взять в расчеты только те границы, при которых системный доход рос, тогда, в setparams можно снять ассерт на это. 
                    if ((middle_rate -> system_income >= start_rate -> system_income) && (finish_rate -> system_income >= middle_rate -> system_income)){
                        eosio::asset incr_amount1 = middle_rate -> system_income - start_rate -> system_income;
                        eosio::asset incr_amount2 = finish_rate -> system_income - middle_rate -> system_income;
                        
                        double ref_amount1 = (double)incr_amount1.amount * (double)ref_quants * ((double)(acc->referral_percent) / (double)(HUNDR_PERCENT)) / (double)sp->size_of_pool / (double)sp -> quants_precision;
                        double dac_amount1 = (double)incr_amount1.amount * (double)ref_quants * (double)(((HUNDR_PERCENT - acc->referral_percent)) / (double)(HUNDR_PERCENT)) / (double)sp->size_of_pool / (double)sp -> quants_precision;   
                        double ref_amount2 = (double)incr_amount2.amount * (double)ref_quants * ((double)(acc->referral_percent) / (double)(HUNDR_PERCENT)) / (double)sp->size_of_pool  / (double)sp -> quants_precision;
                        double dac_amount2 = (double)incr_amount2.amount * (double)ref_quants * (double)(((HUNDR_PERCENT - acc->referral_percent)) / (double)(HUNDR_PERCENT)) / (double)sp->size_of_pool  / (double)sp -> quants_precision;   
                        
                        double total_ref = ref_amount1 + ref_amount2;
                        double total_dac_and_fund = dac_amount1 + dac_amount2;


                        double ref_sys_amount = total_ref * SYS_PERCENT / HUNDR_PERCENT;
                        total_ref = total_ref - ref_sys_amount;
                        double dac_sys_amount = total_dac_and_fund * SYS_PERCENT / HUNDR_PERCENT;
                        total_dac_and_fund = total_dac_and_fund - dac_sys_amount;
                        

                        double total_sys = ref_sys_amount + dac_sys_amount;


                        double total_dac = total_dac_and_fund * acc->dacs_percent / HUNDR_PERCENT; 
                        
                        double total_cfund = total_dac_and_fund - total_dac;
                

                        // uint64_t r_amount = (uint64_t)ref_amount1 + (uint64_t)ref_amount2;
                        // uint64_t d_amount = (uint64_t)dac_amount1 + (uint64_t)dac_amount2;

                        asset_ref_amount = asset(total_ref, root_symbol);
                        asset_dac_amount = asset(total_dac, root_symbol);
                        asset_sys_amount = asset(total_sys, root_symbol);
                        asset_cfund_amount = asset(total_cfund, root_symbol);

                        // print("asset_sys_amount:", asset_sys_amount, " ; ");
                        // print("asset_ref_amount:", asset_ref_amount, " !!! ");
                       
                        } else {
                            asset_ref_amount = asset(0, root_symbol);
                            asset_dac_amount = asset(0, root_symbol);
                            asset_sys_amount = asset(0, root_symbol);
                            asset_cfund_amount = asset(0, root_symbol);
                        }
                        
                        //print("bal->ref_amount: ", bal->ref_amount, " ;");
                        balance.modify(bal, username, [&](auto &b){
                            b.last_recalculated_win_pool_id = i;
                            b.quants_for_sale = new_quants_for_sale;
                            b.next_quants_for_sale = new_reduced_quants;
                            b.available = available;
                            b.win = true;
                            b.if_convert = asset(0,root_symbol);
                            b.forecasts = forecasts;
                            b.ref_amount = bal->ref_amount + asset_ref_amount;
                            b.dac_amount = bal->dac_amount + asset_dac_amount;
                            b.sys_amount = bal->sys_amount + asset_sys_amount;
                            b.cfund_amount = bal->cfund_amount + asset_cfund_amount;
                        });

                } else {
                    //LOSS
                    
                    eosio::asset available = asset(purchase_amount.amount * (HUNDR_PERCENT - sp -> loss_percent) / HUNDR_PERCENT, root_symbol) ;
                    
                    std::vector <eosio::asset> forecasts0 = bal->forecasts;
    
                    if (forecasts0.begin() != forecasts0.end())
                        forecasts0.erase(forecasts0.begin());
                    // forecasts = ecalculate_forecast(username, parent_host, bal->next_quants_for_sale, look_pool -> pool_num - 1, bal->purchase_amount);
                    
                    // forecasts = calculate_forecast(username, parent_host, bal->quants_for_sale, look_pool -> pool_num - 2, bal->purchase_amount);
                        
                    balance.modify(bal, username, [&](auto &b){
                        b.last_recalculated_win_pool_id = i;
                        b.win = false;
                        b.available = available;
                        b.forecasts = forecasts0;
                        b.if_convert = if_convert;
                    });
                    
                };
            }
        }

}

/**
 * @brief      Метод расчета прогнозов
 * Внутренний метод расчета прогнозов выплат для последующих 4х пулов по балансу ядерных Юнитов и на основе будущих курсов. Используются только для демонастрации пользователю. 
 * 
 * 
 * TODO
 * Может дополнительно быть реализован в качестве внешнего метода достоверной проверки прогнозов, который с каждым вызовом производит расчет будущих курсов и расширяет массив с данными по желанию пользователя.
 * Устранить избыток кода.
 * 
 * @param[in]  username  The username
 * @param[in]  host      The host
 * @param[in]  quants    The quants
 * @param[in]  pool_num  The pool number
 * 
 * @return     The forecast.
 */

std::vector <eosio::asset> calculate_forecast(account_name username, account_name host, uint64_t quants, uint64_t pool_num, eosio::asset purchase_amount){
    
    //TODO -> cycle / recursion

    account_index accounts(_self, host);
    auto acc = accounts.find(host);
    auto main_host = acc->get_ahost();

    balance_index balance (_self, username);
    rate_index rates (_self, main_host);
    spiral_index spiral (_self, main_host);

    auto root_symbol = acc->get_root_symbol();
    auto sp = spiral.find(0);

    std::vector<eosio::asset> forcasts;
    uint64_t quants_forecast1;
    uint64_t quants_forecast2;
    uint64_t quants_forecast3;
    uint64_t quants_forecast4;

        


    if (pool_num + 3 < sp->pool_limit ) {
        
        auto rate1 = rates.find(pool_num + 2);
        eosio::asset forecast1 = asset(quants * rate1 -> sell_rate / sp -> quants_precision, root_symbol);
        
        // forcasts.push_back(forecast1);
        quants_forecast1 = quants * rate1 -> sell_rate / rate1 -> buy_rate;

    } 
   
   if (pool_num + 4 < sp->pool_limit){
        auto rate15 = rates.find(pool_num + 3);
        eosio::asset forecast15;
        if (acc->sale_is_enabled == true){
        
            forecast15 = asset((double)quants * (double)rate15->sell_rate / (double)sp->base_rate  / (double)sp->quants_precision * pow(10, acc -> sale_shift), (acc->asset_on_sale).symbol);        
        
        } else {
        
            forecast15 = asset(purchase_amount.amount * (HUNDR_PERCENT - sp -> loss_percent) / HUNDR_PERCENT, root_symbol) ;
        
        }

        forcasts.push_back(forecast15);
    }

   
    if (pool_num + 5 < sp->pool_limit){
       
       auto rate2 = rates.find(pool_num + 4);
       eosio::asset forecast2 = asset(quants_forecast1 * rate2 -> sell_rate / sp -> quants_precision, root_symbol);
       quants_forecast2 = quants_forecast1 * rate2 -> sell_rate / rate2 -> buy_rate;
       forcasts.push_back(forecast2);
    
    } 

     if (pool_num + 6 < sp->pool_limit){
        auto rate25 = rates.find(pool_num + 5);
        eosio::asset forecast25;
        if (acc->sale_is_enabled == true){
        
            forecast25 = asset((double)quants_forecast1 * (double)rate25->sell_rate / (double)sp->base_rate  / (double)sp->quants_precision * pow(10, acc -> sale_shift), (acc->asset_on_sale).symbol);        
        
        } else {
        
            forecast25 = asset(purchase_amount.amount * (HUNDR_PERCENT - sp -> loss_percent) / HUNDR_PERCENT, root_symbol) ;
        
        }

        forcasts.push_back(forecast25);
    }



    if (pool_num + 7 < sp->pool_limit){
        auto rate3 = rates.find(pool_num + 6);
        eosio::asset forecast3 = asset(quants_forecast2 * rate3 -> sell_rate / sp -> quants_precision, root_symbol);
        quants_forecast3 = quants_forecast2 * rate3 -> sell_rate / rate3 -> buy_rate;
       
        forcasts.push_back(forecast3);

    }



    if (pool_num + 8 < sp->pool_limit){
        auto rate35 = rates.find(pool_num + 7);
        eosio::asset forecast35;
        if (acc->sale_is_enabled == true){
        
            forecast35 = asset((double)quants_forecast2 * (double)rate35->sell_rate / (double)sp->base_rate  / (double)sp->quants_precision * pow(10, acc -> sale_shift), (acc->asset_on_sale).symbol);        
        
        } else {
        
            forecast35 = asset(purchase_amount.amount * (HUNDR_PERCENT - sp -> loss_percent) / HUNDR_PERCENT, root_symbol) ;
        
        }

        forcasts.push_back(forecast35);
    }


    if (pool_num + 9 < sp->pool_limit){
        auto rate4 = rates.find(pool_num + 8);
        eosio::asset forecast4 = asset(quants_forecast3 * rate4 -> sell_rate / sp -> quants_precision, root_symbol);
        forcasts.push_back(forecast4);
    }
    
    return forcasts;


};

/**
 * @brief Публичный метод включения сейла с хоста при фонде
 *
*/
void enablesale_action(const enablesale &op){
    // account_name host;
    // account_name token_contract;
    // eosio::asset asset_on_sale;

    require_auth(op.host);

    funds_index funds(_self, _self);
    auto by_contract_and_symbol = funds.template get_index<N(bycontractandsymbol)>();

    auto by_contract_and_symbol_ids = combine_ids(op.token_contract, op.asset_on_sale.symbol);
    
    auto fund = by_contract_and_symbol.find(by_contract_and_symbol_ids);


    eosio_assert(fund != by_contract_and_symbol.end(), "Fund is not exist");
    
    
    account_index accounts(_self, op.host);
    auto acc = accounts.find(op.host);
    eosio_assert(acc != accounts.end(), "Host is not found");
    
    eosio_assert(acc->activated == false, "Sale can be enabled only before launch");

    eosio_assert(acc->sale_is_enabled == false, "Sale is already enabled");

    //Check for host on fund existing


    hostsonfunds_index hostsonfunds(_self, _self);

    auto by_fund_and_host = hostsonfunds.template get_index<N(byfundandhost)>();

    auto by_fund_and_host_ids = combine_ids(fund->id, op.host);
    
    auto host_on_fund = by_fund_and_host.find(by_fund_and_host_ids);

    eosio_assert(host_on_fund != by_fund_and_host.end(), "Host is not connected to fund");

    accounts.modify(acc, op.host, [&](auto &a){
        a.sale_is_enabled = true;
        a.sale_token_contract = op.token_contract;
        a.asset_on_sale = op.asset_on_sale;
        a.sale_shift = op.sale_shift;
    });

}


/**
 * @brief Публичный метод подключения хоста к фонду
 *
*/
void connect_host_to_fund_action(const addhostofund &op){
    
    funds_index funds(_self, _self);

    auto fund = funds.find(op.fund_id);

    eosio_assert(fund != funds.end(), "Fund is not exist");
    
    require_auth(fund -> issuer);
    
    account_index accounts(_self, op.host);
    auto acc = accounts.find(op.host);
    eosio_assert(acc != accounts.end(), "Host is not exist");
    
    hostsonfunds_index hostsonfunds(_self, _self);
    auto by_fund_and_host = hostsonfunds.template get_index<N(byfundandhost)>();

    auto by_fund_and_host_ids = combine_ids(op.fund_id, op.host);
    
    auto host_on_fund = by_fund_and_host.find(by_fund_and_host_ids);


    eosio_assert(host_on_fund == by_fund_and_host.end(), "Host is already connected");
    
    hostsonfunds.emplace(_self, [&](auto &h){
        h.id = hostsonfunds.available_primary_key();
        h.fund_id = fund -> id;
        h.host = op.host;
    });

}


/**
 * @brief Публичный метод пополнения фонда
 *
*/
void add_asset_to_fund_action(account_name username, eosio::asset quantity, account_name code){
    
    require_auth(username);

    funds_index funds(_self, _self);

    auto by_contract_and_symbol = funds.template get_index<N(bycontractandsymbol)>();

    auto by_contract_and_symbol_ids = combine_ids(code, quantity.symbol);
    
    auto fund = by_contract_and_symbol.find(by_contract_and_symbol_ids);

    eosio_assert(fund != by_contract_and_symbol.end(), "Fund is not exist");
    eosio_assert((fund -> fund).symbol == quantity.symbol, "Symbol mismatch");
    eosio_assert(fund -> token_contract == code, "Contract mismatch");


    by_contract_and_symbol.modify(fund, _self, [&](auto &b){
        b.fund = fund -> fund + quantity;
    });
}


/**
 * @brief Публичный метод создания фонда
 *
*/

void createfund_action(const createfund &op){
    stats stat(op.token_contract, op.fund_asset.symbol.name());
    auto s = stat.find(op.fund_asset.symbol.name());
    
    require_auth(s -> issuer);
    
    funds_index funds(_self, _self);

    eosio_assert(op.fund_asset.amount == 0, "Fund amount should be zero on fund creation process");

    auto by_contract_and_symbol = funds.template get_index<N(bycontractandsymbol)>();

    auto by_contract_and_symbol_ids = combine_ids(op.token_contract, op.fund_asset.symbol);
    
    auto fund = by_contract_and_symbol.find(by_contract_and_symbol_ids);

    eosio_assert(fund == by_contract_and_symbol.end(), "Fund already exist");


    funds.emplace(_self, [&](auto &b){
        b.id = funds.available_primary_key();
        b.issuer = s->issuer;
        b.token_contract = op.token_contract;
        b.fund = op.fund_asset;
        b.descriptor = op.descriptor;
    });
}


/**
 * @brief Публичный метод покупки по текущему курсу из числа квантов раунда.
 *
*/

void convert_action(const convert &op){
    require_auth(op.username);

    account_index accounts(_self, op.host);

    pool_index pools(_self, op.host);
    rate_index rates(_self, op.host);
    spiral_index spiral(_self, op.host);

    auto acc = accounts.find(op.host);
    auto sp = spiral.find(0);

    eosio_assert(acc -> sale_is_enabled == true, "Sale is disabled");

    cycle_index cycles(_self, op.host);
    auto cycle = cycles.find(acc -> current_cycle_num - 1);
    
    balance_index balances(_self, op.username);    
    auto bal = balances.find(op.balance_id);
    eosio_assert(bal != balances.end(), "balance is not found");
    eosio_assert(bal->last_recalculated_win_pool_id == cycle->finish_at_global_pool_id, "Cannot convert not refreshed balance. Refresh Balance first and try again.");

    auto rate = rates.find(acc->current_pool_num - 1);

    check_and_modify_sale_fund(bal-> if_convert, *acc);

    action(
        permission_level{ _self, N(active) },
        acc->sale_token_contract, N(transfer),
        std::make_tuple( _self, op.username, bal-> if_convert, std::string("Convert")) 
    ).send();


    add_sale_history(*acc, *rate, *sp, bal-> if_convert, bal->quants_for_sale);       
    
    balances.erase(bal);

}

/**
 * @brief Публичный метод покупки по текущему курсу из числа квантов раунда.
 *
*/

void buy_action(account_name username, account_name host, eosio::asset quantity, account_name code){
    require_auth(username);

    account_index accounts(_self, host);
    auto acc = accounts.find(host);
    
    eosio_assert(acc -> sale_is_enabled == true, "Sale is disabled");

    account_name main_host = acc->get_ahost();
    auto root_symbol = acc->get_root_symbol();

    eosio_assert(code == acc->root_token_contract, "Wrong root token for this host");
    eosio_assert(quantity.symbol == root_symbol, "Wrong root symbol for this host");

    pool_index pools(_self, host);
    rate_index rates(_self, host);

    spiral_index spiral(_self, host);
    auto sp = spiral.find(0);

    // auto cycle = cycles.find(pool -> cycle_num - 1);
    auto pool = pools.find(acc->current_pool_id);
    auto rate = rates.find(acc->current_pool_num - 1);

    auto quant_cost = pool -> quant_cost;
    
    eosio_assert(quantity.amount >= quant_cost.amount, "Not enought quantity for buy 1 unit");
    eosio_assert(quantity.amount % quant_cost.amount == 0, "You can purchase only whole Unit");
    
    auto quants = quantity.amount * sp->quants_precision / rate->buy_rate ;

    eosio_assert(pool->remain_quants >= quants, "Not enought quants in target pool");

    auto amount_in_units = (double)quants * (double)rate->sell_rate / (double)sp->base_rate /  sp->quants_precision * pow(10, acc -> sale_shift);

    eosio::asset units_for_user = asset(amount_in_units, (acc->asset_on_sale).symbol);
    
    pools.modify(pool, _self, [&](auto &p){
        p.remain_quants = pool->remain_quants - quants;
    });

    
    check_and_modify_sale_fund(units_for_user, *acc);

    action(
        permission_level{ _self, N(active) },
        acc->sale_token_contract, N(transfer),
        std::make_tuple( _self, username, units_for_user, std::string("Direct buy")) 
    ).send();

    add_sale_history(*acc, *rate, *sp, units_for_user, quants);
    
    refresh_state(host);
       
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

void withdraw_action ( const withdraw &op){
    //TODO Контроль выплат всем
    //TODO рефакторинг

    //Если системный доход не используется - УДАЛИТЬ

    require_auth(op.username);
    
    auto username = op.username;
    auto balance_id = op.balance_id;
    auto host = op.host;

    account_index accounts(_self, host);
    auto acc = accounts.find(host);
    account_name main_host = acc->get_ahost();
    auto root_symbol = acc->get_root_symbol();

    refresh_state(host);
    
    rate_index rates(_self, main_host);
    pool_index pools(_self, host);
    cycle_index cycles(_self, host);

    balance_index balance (_self, username);
    spiral_index spiral(_self, main_host);
    sincome_index sincome(_self, host);
    std::vector<eosio::asset> forecasts;
    
    
    auto sp = spiral.find(0);
    auto bal = balance.find(balance_id);
    
    eosio_assert(bal != balance.end(), "Balance is not found");

    auto pool = pools.find(bal->global_pool_id);
    auto cycle = cycles.find(pool -> cycle_num - 1);
    
    eosio_assert(bal->last_recalculated_win_pool_id == cycle->finish_at_global_pool_id, "Cannot withdraw not refreshed balance. Refresh Balance first and try again.");

    

    auto next_cycle = cycles.find(pool -> cycle_num);
    auto has_new_cycle = false;
    if (next_cycle != cycles.end())
        has_new_cycle = true;

    auto last_pool = pools.find(cycle -> finish_at_global_pool_id );
    auto rate = rates.find(last_pool -> pool_num - 1 );
    auto prev_rate = rates.find(last_pool -> pool_num - 2);

    eosio_assert(last_pool -> remain_quants <= pool->total_quants, "Prevented withdraw. Only BP can restore this balance");
    
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

    {
        action(
            permission_level{ _self, N(active) },
            acc->root_token_contract, N(transfer),
            std::make_tuple( _self, username, bal -> purchase_amount, std::string("User Withdraw")) 
        ).send();


        if (pool -> pool_num < 3){

            pools.modify(pool, _self, [&](auto &p){
                p.remain_quants = std::min(pool-> remain_quants + bal -> quants_for_sale, pool->total_quants);
            }); 

        } else {

            pools.modify(last_pool, _self, [&](auto &p){
                p.remain_quants = std::min(last_pool-> remain_quants + bal -> quants_for_sale, last_pool->total_quants);
            });

        }

        balance.modify(bal, username, [&](auto &b){
            b.quants_for_sale = 0; 
            b.withdrawed = true;
            b.available = asset(0, root_symbol);
            b.forecasts = forecasts;
        });

        balance.erase(bal);
        
    } else  { 
    /**
     * Выигрыш или проигрыш.
     * Расчет производится на основе сравнения текущего цвета пула с цветом пула входа. При совпадении цветов - баланс выиграл. При несовпадении - проиграл. 
     * 
     */

        auto amount = bal -> available;
        
        if (amount.amount > 0){
            
                action(
                    permission_level{ _self, N(active) },
                    acc->root_token_contract, N(transfer),
                    std::make_tuple( _self, username, amount, std::string("User Withdraw")) 
                ).send();
            
        }
        uint64_t quants_from_reserved;
        if (bal -> win == true){

            /**
             * Выигрыш.
             * Инициирует распределение реферальных вознаграждений и выплаты во все фонды.
             */

            auto converted_quants = bal->quants_for_sale * rate -> sell_rate / rate -> buy_rate;
            
            auto sinc = sincome.find(bal->cycle_num - 1);
           
            if ((bal->ref_amount).amount > 0){
                // print("Total Pay to REF:", bal->ref_amount, " ");
                
                user_index refs(_self, _self);
                auto ref = refs.find(username);
                account_name referer;

                if ((ref != refs.end()) && (ref->referer != 0)){
                    referer = ref->referer;
                    eosio::asset paid = asset(0, root_symbol);
                
                    /**
                     * Вычисляем размер выплаты для каждого уровня партнеров и производим выплаты.
                     */
                
                    for (auto level : acc->levels){
                        if ((ref != refs.end()) && (ref->referer != 0)){
                            eosio::asset to_ref = asset((bal->ref_amount).amount * level / 100 / PERCENT_PRECISION , root_symbol);
                            // print("pay_to_ref: ", to_ref, " ;");
                            // print("pay_to_referer:", referer, " ;");
                            if ((to_ref.amount > 0) && (ref->referer != _self)){
                                action(
                                    permission_level{ _self, N(active) },
                                    acc->root_token_contract, N(transfer),
                                    std::make_tuple( _self, referer, to_ref, std::string("RP-" + (name{username}).to_string() + "-" + (name{referer}).to_string())) 
                                ).send();
                                
          
                                paid += to_ref;
                            
                            };

                            ref = refs.find(referer);
                            referer = ref->referer;
                        }
                    };

                    /**
                     * Все неиспользуемые вознаграждения с вышестояющих уровней отправляются на аккаунт хоста сообщества. 

                     */
                    eosio::asset back_to_host = bal->ref_amount - paid;
                    
                    if (back_to_host.amount>0){
                        action(
                            permission_level{ _self, N(active) },
                            acc->root_token_contract, N(transfer),
                            std::make_tuple( _self, host, back_to_host, std::string("RHP-" + (name{username}.to_string() + "-" + (name{host}).to_string()) ))
                        ).send();
                    };


                    
                } else {
                    /**
                     * Если рефералов у пользователя нет, то переводим все реферальные средства на аккаунт хоста.
                     */
                    action(
                        permission_level{ _self, N(active) },
                        acc->root_token_contract, N(transfer),
                        std::make_tuple( _self, host, bal->ref_amount, std::string("RHP-" + (name{username}.to_string() + "-" + (name{host}).to_string()) )) 
                    ).send();

                }

                /**
                 * Модифицируем объект реферальной статистики для дальнейшей сверки с расчетными данными. 
                 */
                
                sincome.modify(sinc, _self, [&](auto &s){
                    s.paid_to_refs = sinc->paid_to_refs + bal->ref_amount;
                });

            }

            if((bal->dac_amount).amount > 0){

                /**
                 * Выплаты на управляющие аккаунты. В первом приближении для выплат используем единственный аккаунт хоста. Позже можем использовать массив управляющих аккаунтов и условия распределеления согласно любой модели (акции, равенство, и т.д.). 
                 * 
                 
                 */
    
                action(
                    permission_level{ _self, N(active) },
                    acc->root_token_contract, N(transfer),
                    std::make_tuple( _self, host, bal->dac_amount, std::string("DAC-" + (name{username}.to_string() + "-" + (name{host}).to_string()))) 
                ).send();
            
                /**
                 * Объект sincome содержит статистическую информацию о фактическом распределении корпоративных выплат и пополнений целевого фонда.
                 */
                sincome.modify(sinc, _self, [&](auto &s){
                    s.paid_to_dacs = sinc->paid_to_dacs + bal->dac_amount;
                });

            }
            
            if((bal->cfund_amount).amount > 0){

                /**
                 * Выплаты в целевой фонд сообщества
                 * 
                 
                 */

                emission_index emis(_self, host);
                auto emi = emis.find(host);

                emis.modify(emi, username, [&](auto &e){
                    e.fund = emi->fund + bal->cfund_amount;
                });
            

                /**
                 * Объект sincome содержит статистическую информацию о фактическом распределении корпоративных выплат и пополнений целевого фонда.
                 */
                sincome.modify(sinc, _self, [&](auto &s){
                    s.paid_to_cfund = sinc->paid_to_cfund + bal->cfund_amount;
                });

            }
            
            if((bal->sys_amount).amount > 0){
                // Выплаты на системный аккаунт сообщества. 

                action(
                    permission_level{ _self, N(active) },
                    acc->root_token_contract, N(transfer),
                    std::make_tuple( _self, N(eosio.saving), bal->sys_amount, std::string("SYS-" + (name{username}.to_string() + "-" + (name{host}).to_string()))) 
                ).send();
            

                /**
                 * Объект sincome содержит статистическую информацию о фактическом распределении корпоративных выплат и пополнений целевого фонда.
                 */
                sincome.modify(sinc, _self, [&](auto &s){
                    s.paid_to_sys = sinc->paid_to_sys + bal->sys_amount;
                });

            }
            


            /**
             * Все кванты победителей должны быть возвращены в пулы. 
             */

            pools.modify(last_pool, _self, [&](auto &p){
                p.total_win_withdraw = last_pool -> total_win_withdraw + amount;
                p.remain_quants = std::min(last_pool -> remain_quants + converted_quants, last_pool->total_quants);
            });


        
        }
        else {
            // Режим сейла
            // РАЗДАЕМ токен по текущему курсу (автоматическая покупка токенов по текущему курсу)
            pools.modify(last_pool, _self, [&](auto &p){
                 p.total_loss_withdraw = last_pool -> total_loss_withdraw + amount;
            });
            
            if (acc -> sale_is_enabled == true){
                
                check_and_modify_sale_fund(bal->if_convert, *acc);

                action(
                    permission_level{ _self, N(active) },
                    acc->sale_token_contract, N(transfer),
                    std::make_tuple( _self, username, bal->if_convert, std::string("WITHC")) 
                ).send();
            
                add_sale_history(*acc, *rate, *sp, bal->if_convert, bal->quants_for_sale);
            }



        }
            
        /**
         * Модицикация не обязательна, поскольку баланс удаляется. Использовалось при отладке. 
         */
        // balance.modify(bal, username, [&](auto &b){
        //     b.quants_for_sale = 0;  
        //     b.withdrawed = true;
        //     b.available = asset(0, root_symbol);
        //     b.forecasts = forecasts; //[]
        // });

        balance.erase(bal);
    }
};

    void add_sale_history(hosts acc, rate rate, spiral sp, eosio::asset amount, uint64_t quants){
        sale_index sales(_self, acc.username);
        auto sale = sales.find(acc.current_pool_id);
        
        if (sale == sales.end()){
            sales.emplace(_self, [&](auto &s){
                s.pool_id = acc.current_pool_id;
                s.rate = (double)rate.sell_rate / (double)sp.base_rate * pow(10, acc.sale_shift); 
                s.solded = amount;
                s.backed_quants = quants;
            });

        } else {
            sales.modify(sale, _self, [&](auto &s){
                s.solded += amount;
                s.backed_quants += quants;
            });
        }
    }

    void check_and_modify_sale_fund(eosio::asset amount, hosts acc){
        funds_index funds(_self, _self);

        auto funds_by_contract_and_symbol = funds.template get_index<N(bycontractandsymbol)>();

        auto fund_by_contract_and_symbol_ids = combine_ids(acc.sale_token_contract, (acc.asset_on_sale).symbol);
        
        auto fund = funds_by_contract_and_symbol.find(fund_by_contract_and_symbol_ids);

        eosio_assert(fund->fund >= amount, "Not enought units in the fund ");
        eosio_assert((fund->fund).symbol == (amount).symbol, "There is should be a same symbols");

        funds_by_contract_and_symbol.modify(fund, _self, [&](auto &e){
            e.fund = fund->fund - amount;
        });

    }

};

}

[[eosio::action]] void unicore2::init(uint64_t system_income){
    require_auth(_me);

    gpercents_index gpercents(_me, _me.value);
    auto gsys_income = gpercents.find("system"_n.value);

    eosio::check(system_income <= 300000, "System income should be less or equal 30%");

    if (gsys_income == gpercents.end()){
        gpercents.emplace(_me, [&](auto &gp){
            gp.key = "system"_n;
            gp.value = system_income;
        });    
    } else {
        gpercents.modify(gsys_income, _me, [&](auto &g){
            g.value = system_income;
        });
    }
    
}

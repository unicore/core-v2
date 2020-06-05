#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/time.hpp>
#include <eosio/print.hpp>

#include "include/core.hpp"

#include "src/shares.cpp"
#include "src/hosts.cpp"
#include "src/fcore.cpp"
#include "src/goals.cpp"
#include "src/voting.cpp"
#include "src/badges.cpp"
#include "src/tasks.cpp"
#include "src/ipfs.cpp"
#include "src/cms.cpp"

using namespace eosio;


extern "C" {
   
   void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
        if (action == "transfer"_n){
            auto op = eosio::unpack_action_data<eosio::currency::transfer>();
            if (op.from != _self){
                auto subcode = op.memo.substr(0,3);
                auto delimeter = op.memo[3];
                check(delimeter == '-', "Wrong subcode format. Right format: code[3]-parameter");
                auto parameter = op.memo.substr(4, op.memo.length());
                uint64_t subintcode = atoi(subcode.c_str());
                
                //codes:
                //100 - deposit
                //110 - pay for host
                //200 - buyshares
                //300 - goal activate action
                //400 - direct fund emission pool
                //500 - buy data
                //600 - direct buy units
                //666 - direct fund community fund balance for some purposes
                
                switch (subintcode){
                    case 100: {
                        //check for code param inside deposit method
                        //auto cd = eosio::string_to_name(code.c_str());
                        //Deposit in the Core

                        //100-alice.tc-message
                        
                        eosio::name host; 
                        std::string message = "";


                        auto delimeter2 = parameter.find('-');
                
                        if (delimeter2 != -1){
                            auto host_string = op.memo.substr(4, delimeter2);
                            
                            host = eosio::string_to_name(host_string.c_str());
                            
                            message = parameter.substr(delimeter2+1, parameter.length());
                    
                        } else {
                            auto host_string = op.memo.substr(4, parameter.length());
                            host = eosio::string_to_name(host_string.c_str());
                        }
                        
                        core().deposit(op.from, host, op.quantity, code, message);
                        break;
                    }
                    case 110: {
                        //check for code outside
                        //auto cd = eosio::string_to_name(code.c_str());
                        //Pay for upgrade
                        
                        auto host = eosio::string_to_name(parameter.c_str());
                        hosts_struct().pay_for_upgrade(host, op.quantity, code);
                        break;
                    }
                    case 200: {
                        //check for code outside
                        //auto cd = eosio::string_to_name(code.c_str());
                        //Buy Shares
                        auto host = eosio::string_to_name(parameter.c_str());
                        shares().buyshares_action(op.from, host, op.quantity, code);
                        break;
                    }
                    case 300: {
                        //check for code inside
                        //Donation for goal
                        auto delimeter2 = parameter.find('-');
                        std::string parameter2 = parameter.substr(delimeter2+1, parameter.length());
                        
                        auto host = eosio::string_to_name(parameter2.c_str());
                        uint64_t goal_id = atoi(parameter.c_str());    
                        require_auth(op.from);
                        goal().donate_action(op.from, host, goal_id, op.quantity, code);
                        break;
                    }
                    case 400: {
                        //direct fund emission pool
                        
                        auto host = eosio::string_to_name(parameter.c_str());
                        core().fund_emi_pool(op.from, host, op.quantity, code);
                        break;
                    }

                    case 500: {
                        //BUY DATA
                        auto delimeter2 = parameter.find('-');
                        std::string parameter2 = parameter.substr(delimeter2+1, parameter.length());
                        
                        auto owner = eosio::string_to_name(parameter2.c_str());
                        auto buyer = op.from;
                        uint64_t data_id = atoi(parameter.c_str());  
                        require_auth(buyer);  

                        ipfs().buydata_action(owner, data_id, buyer, op.quantity, code);
                        break;
                    }
                    case 600: {
                        //BUY UNITS
                        //direct buy saled units
                        
                        auto host = eosio::string_to_name(parameter.c_str());
                        core().buy_action(op.from, host, op.quantity, code);
                        break;
                    }

                    case 666: {
                        core().add_asset_to_fund_action(op.from, op.quantity, code);
                        break;
                    }

                    // default:
                       //check(false, "Subcode is wrong");
                }

            }
        } else if (code == _self){
            switch (action){
                //GOALS
                 case name(setgoal): {
                    goal().set_goal_action(eosio::unpack_action_data<setgoal>());
                    break;
                 }
                 case name(editgoal): {
                    goal().edit_goal_action(eosio::unpack_action_data<editgoal>());
                    break;
                 }
                 case name(delgoal): {
                    goal().del_goal_action(eosio::unpack_action_data<delgoal>());
                    break;
                 }
                 case name(gsponsor) : {
                    goal().gsponsor_action(eosio::unpack_action_data<gsponsor>());
                    break;
                 }
                 case name(report): {
                    goal().report_action(eosio::unpack_action_data<report>());
                    break;
                 }
                 case name(check) : {
                    goal().check_action(eosio::unpack_action_data<check>());
                    break;
                }

                //CMS
                case name(setcontent): {
                    cms().setcontent_action(eosio::unpack_action_data<setcontent>());
                    break;
                }

                case name(rmcontent): {
                    cms().rmcontent_action(eosio::unpack_action_data<rmcontent>());
                    break;
                }

                case name(setcmsconfig): {
                    cms().setcmsconfig_action(eosio::unpack_action_data<setcmsconfig>());
                    break;
                }


                //IPFS
                case name(setstorage): {
                    ipfs().setstorage_action(eosio::unpack_action_data<setstorage>());
                    break;
                }

                case name(removeroute): {
                    ipfs().removeroute_action(eosio::unpack_action_data<removeroute>());
                    break;
                }

                case name(setipfskey): {
                    ipfs().setipfskey_action(eosio::unpack_action_data<setipfskey>());
                    break;
                }

                case name(selldata): {
                    ipfs().selldata_action(eosio::unpack_action_data<selldata>());
                    break;
                }
                
                case name(dataapprove): {
                    ipfs().orbapprove_action(eosio::unpack_action_data<dataapprove>());
                    break;
                }
               
                //VOTING
                 case name(vote): { 
                    voting().vote_action(eosio::unpack_action_data<vote>());
                    break;
                 }
                //SHARES
                case name(sellshares): {
                    shares().sellshares_action(eosio::unpack_action_data<sellshares>());
                    break;
                };
                case name(delshares): {
                    auto op = eosio::unpack_action_data<delshares>();
                    require_auth(op.from);

                    shares().delegate_shares_action(op);
                    break;
                };
                case name(undelshares): {
                    shares().undelegate_shares_action(eosio::unpack_action_data<undelshares>());
                    break;
                };
                case name(refreshsh): {
                    shares().refresh_action(eosio::unpack_action_data<refreshsh>());
                    break;
                };
                case name(withdrawsh): {
                    shares().withdraw_action(eosio::unpack_action_data<withdrawsh>());
                    break;
                };

                case name(withbenefit): {
                    shares().withdraw_power_units_action(eosio::unpack_action_data<withbenefit>());
                    break;
                };
                case name(refreshpu): {
                    shares().refresh_power_units_action(eosio::unpack_action_data<refreshpu>());
                    break;
                };
                
            

                //HOSTS
                case name(upgrade): {
                    hosts_struct().upgrade_action(eosio::unpack_action_data<upgrade>());
                    break;
                };
                case name(cchildhost): {
                    hosts_struct().create_chost_action(eosio::unpack_action_data<cchildhost>());
                    break;
                };

                //CORE
                case name(setparams): {
                    core().setparams_action(eosio::unpack_action_data<setparams>());
                    break;
                };
                case name(start): {
                   core().start_action(eosio::unpack_action_data<start>());
                   break;
                };
                case name(refreshbal): {
                    core().refresh_balance_action(eosio::unpack_action_data<refreshbal>());
                    break;
                };
                case name(refreshst): {
                    auto op = eosio::unpack_action_data<refreshst>();
                    core().refresh_state(op.host);
                    break;
                };
                
                case name(withdraw): {
                    core().withdraw_action(eosio::unpack_action_data<withdraw>());
                    break;
                };
                case name(reg): {
                    core().reg_action(eosio::unpack_action_data<reg>());
                    break;
                };
                case name(del): {
                    core().del_action(eosio::unpack_action_data<del>());
                    break;
                };
                case name(profupdate): {
                    core().profupdate_action(eosio::unpack_action_data<profupdate>());
                    break;
                };
                case name(priorenter): {
                    core().priority_enter(eosio::unpack_action_data<priorenter>());   
                    break;
                };
                case name(gwithdraw):{
                    goal().gwithdraw_action(eosio::unpack_action_data<gwithdraw>());
                    break;
                };
                case name(setemi):{
                    goal().set_emission_action(eosio::unpack_action_data<setemi>());
                    break;
                };

                case name(edithost):{
                    hosts_struct().edithost_action(eosio::unpack_action_data<edithost>());
                    break;
                };
                case name(createfund):{
                    core().createfund_action(eosio::unpack_action_data<createfund>());
                    break;
                };
                case name(addhostofund):{
                    core().connect_host_to_fund_action(eosio::unpack_action_data<addhostofund>());
                    break;
                };
                case name(enablesale): {
                    core().enablesale_action(eosio::unpack_action_data<enablesale>());
                    break;
                };

            

                //BADGES
                case name(setbadge):{
                    badge_struct().setbadge_action(eosio::unpack_action_data<setbadge>());
                    break;
                }
                // case name(delbadge):{
                //     badge_struct().delbadge_action(eosio::unpack_action_data<delbadge>());
                //     break;
                // }
                case name(giftbadge): {
                    auto op = eosio::unpack_action_data<giftbadge>();

                    require_auth(op.host);

                    badge_struct().giftbadge_action(op);
                    break;
                }
                case name(backbadge): {
                    badge_struct().backbadge_action(eosio::unpack_action_data<backbadge>());
                    break;
                }

                //TASKS

                case name(settask):{
                    tsks().settask_action(eosio::unpack_action_data<settask>());
                    break;
                }

                case name(tactivate):{
                    tsks().tactivate_action(eosio::unpack_action_data<tactivate>());
                    break;
                }
                case name(tdeactivate): {
                    tsks().tdeactivate_action(eosio::unpack_action_data<tdeactivate>());
                    break;
                }
                case name(setreport): {
                    tsks().setreport_action(eosio::unpack_action_data<setreport>());
                    break;
                }


                case name(editreport):{
                    tsks().editreport_action(eosio::unpack_action_data<editreport>());
                    break;
                }

                case name(approver):{
                    tsks().approver_action(eosio::unpack_action_data<approver>());
                    break;
                }

                case name(disapprover):{
                    tsks().disapprover_action(eosio::unpack_action_data<disapprover>());
                    break;
                }

                case name(fundtask) : {
                    tsks().fundtask_action(eosio::unpack_action_data<fundtask>());
                    break;
                }

                case name(setarch) : {
                    hosts_struct().set_architect_action(eosio::unpack_action_data<setarch>());
                    break;
                }

                case name(dfundgoal) : {
                    goal().fund_goal_action(eosio::unpack_action_data<dfundgoal>());
                    break;
                }

                case name(convert) : {
                    core().convert_action(eosio::unpack_action_data<convert>());
                    break;
                }

                case name(fixs) : {
                    core().fixs(eosio::unpack_action_data<fixs>());
                    break;
                }
            }
            
        }
}

}; 

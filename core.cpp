#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/time.hpp>
#include <eosio/print.hpp>

#include "include/core.hpp"
#include "include/eosio.token.hpp"
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
        if (action == "transfer"_n.value){

            struct transfer{
                eosio::name from;
                eosio::name to;
                eosio::asset quantity;
                std::string memo;
            };

            auto op = eosio::unpack_action_data<transfer>();

            // auto op = eosio::unpack_action_data<eosio::token::transfer>();
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
                        //auto cd = name(code.c_str());
                        //Deposit in the Core

                        //100-alice.tc-message
                        
                        eosio::name host; 
                        std::string message = "";


                        auto delimeter2 = parameter.find('-');
                
                        if (delimeter2 != -1){
                            auto host_string = op.memo.substr(4, delimeter2);
                            
                            host = name(host_string.c_str());
                            
                            message = parameter.substr(delimeter2+1, parameter.length());
                    
                        } else {
                            auto host_string = op.memo.substr(4, parameter.length());
                            host = name(host_string.c_str());
                        }
                        
                        core().deposit(op.from, host, op.quantity, name(code), message);
                        break;
                    }
                    case 110: {
                        //check for code outside
                        //auto cd = name(code.c_str());
                        //Pay for upgrade
                        
                        auto host = name(parameter.c_str());
                        hosts_struct().pay_for_upgrade(host, op.quantity, name(code));
                        break;
                    }
                    case 200: {
                        //check for code outside
                        //auto cd = name(code.c_str());
                        //Buy Shares
                        auto host = name(parameter.c_str());
                        shares().buyshares_action(op.from, host, op.quantity, name(code));
                        break;
                    }
                    case 300: {
                        //check for code inside
                        //Donation for goal
                        auto delimeter2 = parameter.find('-');
                        std::string parameter2 = parameter.substr(delimeter2+1, parameter.length());
                        
                        auto host = name(parameter2.c_str());
                        uint64_t goal_id = atoi(parameter.c_str());    
                        require_auth(op.from);
                        goal().donate_action(op.from, host, goal_id, op.quantity, name(code));
                        break;
                    }
                    case 400: {
                        //direct fund emission pool
                        
                        auto host = name(parameter.c_str());
                        core().fund_emi_pool(op.from, host, op.quantity, name(code));
                        break;
                    }

                    case 500: {
                        //BUY DATA
                        auto delimeter2 = parameter.find('-');
                        std::string parameter2 = parameter.substr(delimeter2+1, parameter.length());
                        
                        auto owner = name(parameter2.c_str());
                        auto buyer = op.from;
                        uint64_t data_id = atoi(parameter.c_str());  
                        require_auth(buyer);  

                        ipfs().buydata_action(owner, data_id, buyer, op.quantity, name(code));
                        break;
                    }
                    case 600: {
                        //BUY UNITS
                        //direct buy saled units
                        
                        auto host = name(parameter.c_str());
                        core().buy_action(op.from, host, op.quantity, name(code));
                        break;
                    }

                    case 666: {
                        core().add_asset_to_fund_action(op.from, op.quantity, name(code));
                        break;
                    }

                    // default:
                       //check(false, "Subcode is wrong");
                }

            }
        } else if (code == _self.value){
            switch (action){
                //GOALS
                 case "setgoal"_n.value: {
                    goal().set_goal_action(eosio::unpack_action_data<setgoal>());
                    break;
                 }
                 case "editgoal"_n.value: {
                    goal().edit_goal_action(eosio::unpack_action_data<editgoal>());
                    break;
                 }
                 case "delgoal"_n.value: {
                    goal().del_goal_action(eosio::unpack_action_data<delgoal>());
                    break;
                 }
                 case "gsponsor"_n.value : {
                    goal().gsponsor_action(eosio::unpack_action_data<gsponsor>());
                    break;
                 }
                 case "report"_n.value: {
                    goal().report_action(eosio::unpack_action_data<report>());
                    break;
                 }
                 case "check"_n.value : {
                    goal().check_action(eosio::unpack_action_data<struct check>());
                    break;
                }

                //CMS
                case "setcontent"_n.value: {
                    cms().setcontent_action(eosio::unpack_action_data<setcontent>());
                    break;
                }

                case "rmcontent"_n.value: {
                    cms().rmcontent_action(eosio::unpack_action_data<rmcontent>());
                    break;
                }

                case "setcmsconfig"_n.value: {
                    cms().setcmsconfig_action(eosio::unpack_action_data<setcmsconfig>());
                    break;
                }


                //IPFS
                case "setstorage"_n.value: {
                    ipfs().setstorage_action(eosio::unpack_action_data<setstorage>());
                    break;
                }

                case "removeroute"_n.value: {
                    ipfs().removeroute_action(eosio::unpack_action_data<removeroute>());
                    break;
                }

                case "setipfskey"_n.value: {
                    ipfs().setipfskey_action(eosio::unpack_action_data<setipfskey>());
                    break;
                }

                case "selldata"_n.value: {
                    ipfs().selldata_action(eosio::unpack_action_data<selldata>());
                    break;
                }
                
                case "dataapprove"_n.value: {
                    ipfs().orbapprove_action(eosio::unpack_action_data<dataapprove>());
                    break;
                }
               
                //VOTING
                 case "vote"_n.value: { 
                    voting().vote_action(eosio::unpack_action_data<vote>());
                    break;
                 }
                //SHARES
                case "sellshares"_n.value: {
                    shares().sellshares_action(eosio::unpack_action_data<sellshares>());
                    break;
                };
                case "delshares"_n.value: {
                    auto op = eosio::unpack_action_data<delshares>();
                    require_auth(op.from);

                    shares().delegate_shares_action(op);
                    break;
                };
                case "undelshares"_n.value: {
                    shares().undelegate_shares_action(eosio::unpack_action_data<undelshares>());
                    break;
                };
                case "refreshsh"_n.value: {
                    shares().refresh_action(eosio::unpack_action_data<refreshsh>());
                    break;
                };
                case "withdrawsh"_n.value: {
                    shares().withdraw_action(eosio::unpack_action_data<withdrawsh>());
                    break;
                };

                case "withbenefit"_n.value: {
                    shares().withdraw_power_units_action(eosio::unpack_action_data<withbenefit>());
                    break;
                };
                case "refreshpu"_n.value: {
                    shares().refresh_power_units_action(eosio::unpack_action_data<refreshpu>());
                    break;
                };
                
            

                //HOSTS
                case "upgrade"_n.value: {
                    hosts_struct().upgrade_action(eosio::unpack_action_data<upgrade>());
                    break;
                };
                case "cchildhost"_n.value: {
                    hosts_struct().create_chost_action(eosio::unpack_action_data<cchildhost>());
                    break;
                };

                //CORE
                case "setparams"_n.value: {
                    core().setparams_action(eosio::unpack_action_data<setparams>());
                    break;
                };
                case "start"_n.value: {
                   core().start_action(eosio::unpack_action_data<start>());
                   break;
                };
                case "refreshbal"_n.value: {
                    core().refresh_balance_action(eosio::unpack_action_data<refreshbal>());
                    break;
                };
                case "refreshst"_n.value: {
                    auto op = eosio::unpack_action_data<refreshst>();
                    core().refresh_state(op.host);
                    break;
                };
                
                case "withdraw"_n.value: {
                    core().withdraw_action(eosio::unpack_action_data<withdraw>());
                    break;
                };
                case "reg"_n.value: {
                    core().reg_action(eosio::unpack_action_data<reg>());
                    break;
                };
                case "del"_n.value: {
                    core().del_action(eosio::unpack_action_data<del>());
                    break;
                };
                case "profupdate"_n.value: {
                    core().profupdate_action(eosio::unpack_action_data<profupdate>());
                    break;
                };
                case "priorenter"_n.value: {
                    core().priority_enter(eosio::unpack_action_data<priorenter>());   
                    break;
                };
                case "gwithdraw"_n.value:{
                    goal().gwithdraw_action(eosio::unpack_action_data<gwithdraw>());
                    break;
                };
                case "setemi"_n.value:{
                    goal().set_emission_action(eosio::unpack_action_data<setemi>());
                    break;
                };

                case "edithost"_n.value:{
                    hosts_struct().edithost_action(eosio::unpack_action_data<edithost>());
                    break;
                };
                case "createfund"_n.value:{
                    core().createfund_action(eosio::unpack_action_data<createfund>());
                    break;
                };
                case "addhostofund"_n.value:{
                    core().connect_host_to_fund_action(eosio::unpack_action_data<addhostofund>());
                    break;
                };
                case "enablesale"_n.value: {
                    core().enablesale_action(eosio::unpack_action_data<enablesale>());
                    break;
                };

            

                //BADGES
                case "setbadge"_n.value:{
                    badge_struct().setbadge_action(eosio::unpack_action_data<setbadge>());
                    break;
                }
                // case name(delbadge):{
                //     badge_struct().delbadge_action(eosio::unpack_action_data<delbadge>());
                //     break;
                // }
                case "giftbadge"_n.value: {
                    auto op = eosio::unpack_action_data<giftbadge>();

                    require_auth(op.host);

                    badge_struct().giftbadge_action(op);
                    break;
                }
                case "backbadge"_n.value: {
                    badge_struct().backbadge_action(eosio::unpack_action_data<backbadge>());
                    break;
                }

                //TASKS

                case "settask"_n.value:{
                    tsks().settask_action(eosio::unpack_action_data<settask>());
                    break;
                }

                case "tactivate"_n.value:{
                    tsks().tactivate_action(eosio::unpack_action_data<tactivate>());
                    break;
                }
                case "tdeactivate"_n.value: {
                    tsks().tdeactivate_action(eosio::unpack_action_data<tdeactivate>());
                    break;
                }
                case "setreport"_n.value: {
                    tsks().setreport_action(eosio::unpack_action_data<setreport>());
                    break;
                }


                case "editreport"_n.value:{
                    tsks().editreport_action(eosio::unpack_action_data<editreport>());
                    break;
                }

                case "approver"_n.value:{
                    tsks().approver_action(eosio::unpack_action_data<approver>());
                    break;
                }

                case "disapprover"_n.value:{
                    tsks().disapprover_action(eosio::unpack_action_data<disapprover>());
                    break;
                }

                case "fundtask"_n.value : {
                    tsks().fundtask_action(eosio::unpack_action_data<fundtask>());
                    break;
                }

                case "setarch"_n.value : {
                    hosts_struct().set_architect_action(eosio::unpack_action_data<setarch>());
                    break;
                }

                case "dfundgoal"_n.value : {
                    goal().fund_goal_action(eosio::unpack_action_data<dfundgoal>());
                    break;
                }

                case "convert"_n.value : {
                    core().convert_action(eosio::unpack_action_data<convert>());
                    break;
                }

                case "fixs"_n.value : {
                    core().fixs(eosio::unpack_action_data<fixs>());
                    break;
                }
            }
            
        }
}

}; 

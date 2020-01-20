#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/print.hpp>
#include "protocol.hpp"
#include "shares.cpp"
#include "hosts.cpp"

#include "core.cpp"
#include "goals.cpp"

#include "voting.cpp"
#include "badges.cpp"
#include "tasks.cpp"
#include "ipfs.cpp"
#include "cms.cpp"

using namespace eosio;


extern "C" {
   
   void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
        if (action == N(transfer)){
            auto op = eosio::unpack_action_data<eosio::currency::transfer>();
            if (op.from != _self){
                auto subcode = op.memo.substr(0,3);
                auto delimeter = op.memo[3];
                eosio_assert(delimeter == '-', "Wrong subcode format. Right format: code[3]-parameter");
                auto parameter = op.memo.substr(4, op.memo.length());
                uint64_t subintcode = atoi(subcode.c_str());
                print("subcode", subintcode);

                //codes:
                //100 - deposit
                //110 - pay for host
                //200 - buyshares
                //300 - goal activate action
                //400 - direct fund emission pool
                //500 - buy data
                //666 - direct fund core balance for some purposes
                
                switch (subintcode){
                    case 100: {
                        //check for code inside
                        //auto cd = eosio::string_to_name(code.c_str());
                        //Deposit in the Core
                        auto host = eosio::string_to_name(parameter.c_str());
                        core().deposit(op.from, host, op.quantity, code);
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
                    case 666: {
                        //direct fund balance for some purposes
                        break;
                    }
                    // default:
                       //eosio_assert(false, "Subcode is wrong");
                }

            }
        } else if (code == _self){
            switch (action){
                //GOALS
                 case N(setgoal): {
                    goal().set_goal_action(eosio::unpack_action_data<setgoal>());
                    break;
                 }
                 case N(editgoal): {
                    goal().edit_goal_action(eosio::unpack_action_data<editgoal>());
                    break;
                 }
                 case N(delgoal): {
                    goal().del_goal_action(eosio::unpack_action_data<delgoal>());
                    break;
                 }
                 case N(gsponsor) : {
                    goal().gsponsor_action(eosio::unpack_action_data<gsponsor>());
                    break;
                 }
                 case N(report): {
                    goal().report_action(eosio::unpack_action_data<report>());
                    break;
                 }
                 case N(check) : {
                    goal().check_action(eosio::unpack_action_data<check>());
                    break;
                }

                //CMS
                case N(setcontent): {
                    cms().setcontent_action(eosio::unpack_action_data<setcontent>());
                    break;
                }

                case N(rmcontent): {
                    cms().rmcontent_action(eosio::unpack_action_data<rmcontent>());
                    break;
                }

                case N(setcmsconfig): {
                    cms().setcmsconfig_action(eosio::unpack_action_data<setcmsconfig>());
                    break;
                }


                //IPFS
                case N(setstorage): {
                    ipfs().setstorage_action(eosio::unpack_action_data<setstorage>());
                    break;
                }

                case N(removeroute): {
                    ipfs().removeroute_action(eosio::unpack_action_data<removeroute>());
                    break;
                }

                case N(setipfskey): {
                    ipfs().setipfskey_action(eosio::unpack_action_data<setipfskey>());
                    break;
                }

                case N(selldata): {
                    ipfs().selldata_action(eosio::unpack_action_data<selldata>());
                    break;
                }
                
                case N(dataapprove): {
                    ipfs().orbapprove_action(eosio::unpack_action_data<dataapprove>());
                    break;
                }
               
                //VOTING
                 case N(vote): { 
                    voting().vote_action(eosio::unpack_action_data<vote>());
                    break;
                 }
                //SHARES
                case N(sellshares): {
                    shares().sellshares_action(eosio::unpack_action_data<sellshares>());
                    break;
                };
                case N(delshares): {
                    auto op = eosio::unpack_action_data<delshares>();
                    require_auth(op.from);

                    shares().delegate_shares_action(op);
                    break;
                };
                case N(undelshares): {
                    shares().undelegate_shares_action(eosio::unpack_action_data<undelshares>());
                    break;
                };
                case N(refreshsh): {
                    shares().refresh_action(eosio::unpack_action_data<refreshsh>());
                    break;
                };
                case N(withdrawsh): {
                    shares().withdraw_action(eosio::unpack_action_data<withdrawsh>());
                    break;
                };
                //HOSTS
                case N(upgrade): {
                    hosts_struct().upgrade_action(eosio::unpack_action_data<upgrade>());
                    break;
                };
                case N(cchildhost): {
                    hosts_struct().create_chost_action(eosio::unpack_action_data<cchildhost>());
                    break;
                };

                //CORE
                case N(setparams): {
                    core().setparams_action(eosio::unpack_action_data<setparams>());
                    break;
                };
                case N(start): {
                   core().start_action(eosio::unpack_action_data<start>());
                   break;
                };
                case N(refreshbal): {
                    core().refresh_balance_action(eosio::unpack_action_data<refreshbal>());
                    break;
                };
                case N(refreshst): {
                    auto op = eosio::unpack_action_data<refreshst>();
                    core().refresh_state(op.host);
                    break;
                };
                
                case N(withdraw): {
                    core().withdraw_action(eosio::unpack_action_data<withdraw>());
                    break;
                };
                case N(reg): {
                    core().reg_action(eosio::unpack_action_data<reg>());
                    break;
                };
                case N(profupdate): {
                    core().profupdate_action(eosio::unpack_action_data<profupdate>());
                    break;
                };
                case N(priorenter): {
                    core().priority_enter(eosio::unpack_action_data<priorenter>());   
                    break;
                };
                case N(gwithdraw):{
                    goal().gwithdraw_action(eosio::unpack_action_data<gwithdraw>());
                    break;
                };
                case N(setemi):{
                    goal().set_emission_action(eosio::unpack_action_data<setemi>());
                    break;
                };

                case N(edithost):{
                    hosts_struct().edithost_action(eosio::unpack_action_data<edithost>());
                    break;
                };

            
                //BADGES
                case N(setbadge):{
                    badge_struct().setbadge_action(eosio::unpack_action_data<setbadge>());
                    break;
                }
                // case N(delbadge):{
                //     badge_struct().delbadge_action(eosio::unpack_action_data<delbadge>());
                //     break;
                // }
                case N(giftbadge): {
                    auto op = eosio::unpack_action_data<giftbadge>();

                    require_auth(op.host);

                    badge_struct().giftbadge_action(op);
                    break;
                }
                // case N(backbadge): {
                //     badge_struct().backbadge_action(eosio::unpack_action_data<backbadge>());
                // }

                //TASKS

                case N(settask):{
                    tsks().settask_action(eosio::unpack_action_data<settask>());
                    break;
                }

                case N(tactivate):{
                    tsks().tactivate_action(eosio::unpack_action_data<tactivate>());
                    break;
                }
                case N(tdeactivate): {
                    tsks().tdeactivate_action(eosio::unpack_action_data<tdeactivate>());
                    break;
                }
                case N(setreport): {
                    tsks().setreport_action(eosio::unpack_action_data<setreport>());
                    break;
                }


                case N(editreport):{
                    tsks().editreport_action(eosio::unpack_action_data<editreport>());
                    break;
                }

                case N(approver):{
                    tsks().approver_action(eosio::unpack_action_data<approver>());
                    break;
                }

                case N(disapprover):{
                    tsks().disapprover_action(eosio::unpack_action_data<disapprover>());
                    break;
                }

                case N(fundtask) : {
                    tsks().fundtask_action(eosio::unpack_action_data<fundtask>());
                    break;
                }

                case N(setarch) : {
                    hosts_struct().set_architect_action(eosio::unpack_action_data<setarch>());
                    break;
                }

                case N(dfundgoal) : {
                    goal().fund_goal_action(eosio::unpack_action_data<dfundgoal>());
                    break;
                }
            }
            
        }
}

}; 

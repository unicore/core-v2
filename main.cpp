#include "include/core.hpp"


#include "src/shares.cpp"
#include "src/hosts.cpp"
#include "src/ref.cpp"
#include "src/core.cpp"
#include "src/goals.cpp"
#include "src/voting.cpp"
#include "src/badges.cpp"
#include "src/tasks.cpp"
#include "src/ipfs.cpp"
#include "src/cms.cpp"
#include "src/conditions.cpp"

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
            if (op.from != _me){
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
                //600 - direct buy quants
                //666 - direct fund commquanty fund balance for some purposes
                
                switch (subintcode){
                    case 100: {
                        //check for code param inside deposit method
                        //100-alice-message
                        
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
                        
                        unicore::deposit(op.from, host, op.quantity, name(code), message);
                        break;
                    }
                    case 110: {
                        //check for code outside
                        //auto cd = name(code.c_str());
                        //Pay for upgrade
                        
                        auto host = name(parameter.c_str());
                        unicore::pay_for_upgrade(host, op.quantity, name(code));
                        break;
                    }
                    case 200: {
                        //check for code outside
                        //auto cd = name(code.c_str());
                        //Buy Shares
                        auto host = name(parameter.c_str());
                        unicore::buyshares_action(op.from, host, op.quantity, name(code));
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

                        unicore::donate_action(op.from, host, goal_id, op.quantity, name(code));
                        break;
                    }
                    case 400: {
                        //direct fund emission pool
                        
                        auto host = name(parameter.c_str());
                        unicore::fund_emi_pool(op.from, host, op.quantity, name(code));
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

                        unicore::buydata_action(owner, data_id, buyer, op.quantity, name(code));
                        break;
                    }
                    case 600: {
                        //BUY QUANTS
                        //direct buy saled quants
                        require_auth(op.from);

                        auto host = name(parameter.c_str());
                        
                        unicore::buy_action(op.from, host, op.quantity, name(code), true);

                        break;
                    }

                    case 666:{
                        // execute_action(name(receiver), name(code), &unicore::createfund);

                        auto delimeter2 = parameter.find('-');
                        std::string parameter2 = parameter.substr(delimeter2+1, parameter.length());
                        
                        auto descriptor = parameter2.c_str();
                        

                        unicore::createfund(name(code), op.quantity, descriptor);

                        // fcore().createfund_action(eosio::unpack_action_data<createfund>());
                        break;
                    };
                
                    // case 700: {
                    //     break;
                    // }

                    default:
                        break;
                       
                }

            }
        } else if (code == _me.value){
            switch (action){
                 case "init"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::init);
                    break;
                 }
                
                //GOALS
                 case "setgoal"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::setgoal);
                    // goal().set_goal_action(eosio::unpack_action_data<setgoal>());
                    break;
                 }
                 case "editgoal"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::editgoal);
                    // goal().edit_goal_action(eosio::unpack_action_data<editgoal>());
                    break;
                 }
                 case "delgoal"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::delgoal);
                    // goal().del_goal_action(eosio::unpack_action_data<delgoal>());
                    break;
                 }
                 case "gsponsor"_n.value : {
                    execute_action(name(receiver), name(code), &unicore::gsponsor);
                    // goal().gsponsor_action(eosio::unpack_action_data<gsponsor>());
                    break;
                 }
                 case "report"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::report);
                    // goal().report_action(eosio::unpack_action_data<report>());
                    break;
                 }
                 case "check"_n.value : {
                    execute_action(name(receiver), name(code), &unicore::check);
                    // goal().check_action(eosio::unpack_action_data<struct check>());
                    break;
                }

                //CMS
                case "setcontent"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::setcontent);
                    // cms().setcontent_action(eosio::unpack_action_data<setcontent>());
                    break;
                }

                case "rmcontent"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::rmcontent);
                    // cms().rmcontent_action(eosio::unpack_action_data<rmcontent>());
                    break;
                }

                case "setcmsconfig"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::setcmsconfig);
                    // cms().setcmsconfig_action(eosio::unpack_action_data<setcmsconfig>());
                    break;
                }


                //IPFS
                case "setstorage"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::setstorage);
                    // ipfs().setstorage_action(eosio::unpack_action_data<setstorage>());
                    break;
                }

                case "removeroute"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::removeroute);
                    // ipfs().removeroute_action(eosio::unpack_action_data<removeroute>());
                    break;
                }

                case "setipfskey"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::setipfskey);
                    // ipfs().setipfskey_action(eosio::unpack_action_data<setipfskey>());
                    break;
                }

                case "selldata"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::selldata);
                    // ipfs().selldata_action(eosio::unpack_action_data<selldata>());
                    break;
                }
                
                case "dataapprove"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::dataapprove);
                    // ipfs().orbapprove_action(eosio::unpack_action_data<dataapprove>());
                    break;
                }
               
                //VOTING
                 case "vote"_n.value: { 
                    execute_action(name(receiver), name(code), &unicore::vote);
                    // voting().vote_action(eosio::unpack_action_data<vote>());
                    break;
                 }
                //SHARES
                case "sellshares"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::sellshares);
                    // shares().sellshares_action(eosio::unpack_action_data<sellshares>());
                    break;
                };
                case "delshares"_n.value: {
                    //?????????????????

                    struct delsharesstruct {
                        eosio::name from; 
                        eosio::name reciever; 
                        eosio::name host;
                        uint64_t shares;
                    };

                    auto op = eosio::unpack_action_data<delsharesstruct>();
                    
                    require_auth(op.from);

                    // execute_action(name(receiver), name(code), &unicore::delshares);
                    unicore::delegate_shares_action(op.from, op.reciever, op.host, op.shares);

                    // shares().delegate_shares_action(op);
                    break;
                };
                case "undelshares"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::undelshares);
                    // shares().undelegate_shares_action(eosio::unpack_action_data<undelshares>());
                    break;
                };
                case "refreshsh"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::refreshsh);
                    // shares().refresh_action(eosio::unpack_action_data<refreshsh>());
                    break;
                };

                case "withdrawsh"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::withdrawsh);
                    // shares().withdraw_action(eosio::unpack_action_data<withdrawsh>());
                    break;
                };

                case "withpbenefit"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::withpbenefit);
                    // shares().withdraw_power_quants_action(eosio::unpack_action_data<withbenefit>());
                    break;
                };
                case "withrbenefit"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::withrbenefit);
                    // shares().withdraw_power_quants_action(eosio::unpack_action_data<withbenefit>());
                    break;
                };
                case "withrsegment"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::withrsegment);
                    
                    break;  
                }
                case "refreshpu"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::refreshpu);
                    // shares().refresh_power_quants_action(eosio::unpack_action_data<refreshpu>());
                    break;
                };
                
            

                //HOSTS
                case "upgrade"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::upgrade);
                    // hosts_struct().upgrade_action(eosio::unpack_action_data<upgrade>());
                    break;
                };
                case "cchildhost"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::cchildhost);
                    // hosts_struct().create_chost_action(eosio::unpack_action_data<cchildhost>());
                    break;
                };
                case "setwebsite"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::setwebsite);
                    // hosts_struct().upgrade_action(eosio::unpack_action_data<upgrade>());
                    break;
                };
                case "settype"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::settype);
                    // hosts_struct().upgrade_action(eosio::unpack_action_data<upgrade>());
                    break;
                };

                //CORE
                case "setparams"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::setparams);

                    // fcore().setparams_action(eosio::unpack_action_data<setparams>());
                    break;
                };
                case "start"_n.value: {
                   execute_action(name(receiver), name(code), &unicore::start);
                   // fcore().start_action(eosio::unpack_action_data<start>());
                   break;
                };
                case "refreshbal"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::refreshbal);
                    // fcore().refresh_balance_action(eosio::unpack_action_data<refreshbal>());
                    break;
                };
                case "refreshst"_n.value: {
                    // execute_action(name(receiver), name(code), &unicore::refreshst);
                    
                    struct refreshstruct {
                        eosio::name host;
                    };
                    
                    auto op = eosio::unpack_action_data<refreshstruct>();
                    
                    unicore::refresh_state(op.host);
                    break;
                };
                
                case "withdraw"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::withdraw);
                    // fcore().withdraw_action(eosio::unpack_action_data<withdraw>());
                    break;
                };
                case "priorenter"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::priorenter);
                    // fcore().priority_enter(eosio::unpack_action_data<priorenter>());   
                    break;
                };
                case "gwithdraw"_n.value:{
                    execute_action(name(receiver), name(code), &unicore::gwithdraw);
                    // goal().gwithdraw_action(eosio::unpack_action_data<gwithdraw>());
                    break;
                };
                case "setemi"_n.value:{
                    execute_action(name(receiver), name(code), &unicore::setemi);
                    // goal().set_emission_action(eosio::unpack_action_data<setemi>());
                    break;
                };

                case "edithost"_n.value:{
                    execute_action(name(receiver), name(code), &unicore::edithost);
                    // hosts_struct().edithost_action(eosio::unpack_action_data<edithost>());
                    break;
                };

                case "setcondition"_n.value:{
                    execute_action(name(receiver), name(code), &unicore::setcondition);
                    // hosts_struct().edithost_action(eosio::unpack_action_data<edithost>());
                    break;
                };

                //POT
                case "addhostofund"_n.value:{
                    execute_action(name(receiver), name(code), &unicore::addhostofund);
                    // fcore().connect_host_to_fund_action(eosio::unpack_action_data<addhostofund>());
                    break;
                };
                case "enablesale"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::enablesale);
                    // fcore().enablesale_action(eosio::unpack_action_data<enablesale>());
                    break;
                };
                case "transfromgf"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::transfromgf);
                    // fcore().enablesale_action(eosio::unpack_action_data<enablesale>());
                    break;
                };
            
                //BADGES
                case "setbadge"_n.value:{
                    execute_action(name(receiver), name(code), &unicore::setbadge);
                    // badge_struct().setbadge_action(eosio::unpack_action_data<setbadge>());
                    break;
                }
                // case name(delbadge):{
                //     badge_struct().delbadge_action(eosio::unpack_action_data<delbadge>());
                //     break;
                // }
                case "giftbadge"_n.value: {

                    struct giftbadge_struct {
                        eosio::name host;
                        eosio::name to;
                        uint64_t badge_id;
                        eosio::string comment;
                    };

                    auto op = eosio::unpack_action_data<giftbadge_struct>();

                    require_auth(op.host);

                    unicore::giftbadge_action(op.host, op.to, op.badge_id, op.comment);

                    break;
                }
                case "backbadge"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::backbadge);
                    // badge_struct().backbadge_action(eosio::unpack_action_data<backbadge>());
                    break;
                }

                //TASKS

                case "settask"_n.value:{
                    execute_action(name(receiver), name(code), &unicore::settask);
                    // tsks().settask_action(eosio::unpack_action_data<settask>());
                    break;
                }

                case "tactivate"_n.value:{
                    execute_action(name(receiver), name(code), &unicore::tactivate);
                    // tsks().tactivate_action(eosio::unpack_action_data<tactivate>());
                    break;
                }
                case "tdeactivate"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::tdeactivate);
                    // tsks().tdeactivate_action(eosio::unpack_action_data<tdeactivate>());
                    break;
                }
                case "setreport"_n.value: {
                    execute_action(name(receiver), name(code), &unicore::setreport);
                    // tsks().setreport_action(eosio::unpack_action_data<setreport>());
                    break;
                }


                case "editreport"_n.value:{
                    execute_action(name(receiver), name(code), &unicore::editreport);
                    // tsks().editreport_action(eosio::unpack_action_data<editreport>());
                    break;
                }

                case "approver"_n.value:{
                    execute_action(name(receiver), name(code), &unicore::approver);
                    // tsks().approver_action(eosio::unpack_action_data<approver>());
                    break;
                }

                case "disapprover"_n.value:{
                    execute_action(name(receiver), name(code), &unicore::disapprover);
                    break;
                }

                case "fundtask"_n.value : {
                    execute_action(name(receiver), name(code), &unicore::fundtask);
                    // tsks().fundtask_action(eosio::unpack_action_data<fundtask>());
                    break;
                }

                case "setarch"_n.value : {
                    execute_action(name(receiver), name(code), &unicore::setarch);
                    // hosts_struct().set_architect_action(eosio::unpack_action_data<setarch>());
                    break;
                }

                case "dfundgoal"_n.value : {
                    execute_action(name(receiver), name(code), &unicore::dfundgoal);
                    // goal().fund_goal_action(eosio::unpack_action_data<dfundgoal>());
                    break;
                }

                case "convert"_n.value : {
                    execute_action(name(receiver), name(code), &unicore::convert);
                    // fcore().convert_action(eosio::unpack_action_data<convert>());
                    break;
                }

                case "adddac"_n.value : {
                    execute_action(name(receiver), name(code), &unicore::adddac);
                    break;
                }
                case "rmdac"_n.value : {
                    execute_action(name(receiver), name(code), &unicore::rmdac);
                    break;
                }
                
                case "withdrdacinc"_n.value : {
                    execute_action(name(receiver), name(code), &unicore::withdrdacinc);
                    break;
                }

                case "fixs"_n.value : {
                    execute_action(name(receiver), name(code), &unicore::fixs);
                    break;
                }
            }
            
        }
}

}; 

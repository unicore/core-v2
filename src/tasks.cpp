
/**
 * @brief      Модуль задач
 * Задачи есть составляющие части достижения любой цели. 
 * Постановка задач осуществляется только в рамках целей. 
 * Задачи могут быть 
 * 	- публичными, т.е. достуными к исполнению каждым участником сообщества, 
 * 	- приватными, т.е. доступными к исполнению только внутренним управляющим командам.
 */

	/**
	 * @brief      Публичный метод создания задачи
	 * Может использовать только аккаунт хоста на текущий момент. 
	 *
	 * @param[in]  op    The operation
	 */
	[[eosio::action]] void unicore::settask(eosio::name host, eosio::name creator, std::string permlink, uint64_t goal_id, uint64_t priority, eosio::string title, eosio::string data, eosio::asset requested, bool is_public, eosio::name doer, eosio::asset for_each, bool with_badge, uint64_t badge_id, uint64_t duration, uint64_t expiration, bool is_batch, uint64_t parent_batch_id, bool is_regular, std::vector<uint64_t> calendar, eosio::time_point_sec start_at, std::string meta){
		
		require_auth(creator);
		
		account_index accounts(_me, host.value);
		auto acc = accounts.find(host.value);
		eosio::check(acc != accounts.end(), "Host is not found");


		auto root_symbol = acc->get_root_symbol();
		
		if (is_public == true) {
			eosio::check(requested.symbol == root_symbol, "Wrong token for current host" );
			eosio::check(for_each.symbol == root_symbol, "Wrong token for current host" );
		} else {
			eosio::check(for_each.amount == 0, "Wrong amount for current host and task mode" );
		}
		
		goals_index goals(_me, host.value);
		auto goal = goals.find(goal_id);
		eosio::check(goal != goals.end(), "Goal is not found");
		

		validate_permlink(permlink);
    
    //check for uniq permlink
    auto hash = hash64(permlink);
    
    std::vector<uint64_t> batch;
		tasks_index tasks(_me, host.value);
		
		if (with_badge == true) {
			badge_index badges(_me, host.value);
			auto badge = badges.find(badge_id);
			eosio::check(badge != badges.end(), "Badge with current ID is not found");
		}

		auto exist_task = tasks.find(hash);

		if (exist_task == tasks.end()) {

			if (parent_batch_id != 0) {
				auto parent_task = tasks.find(parent_batch_id);
				eosio::check(parent_task != tasks.end(), "Parent batch is not founded");
				// eosio::check(parent_task -> is_batch == true, "Parent task is not a batch");

				batch = parent_task -> batch;
				
				//TODO check for batch length
				auto itr = std::find(batch.begin(), batch.end(), hash);
				
				if (itr == batch.end()){
					batch.push_back(hash);
				};

				tasks.modify(parent_task, creator, [&](auto &t){
					t.batch = batch;
				});

			};

			if (goal != goals.end()) {
				eosio::check(goal -> who_can_create_tasks == creator || goal->who_can_create_tasks.value == 0, "Only creator can create task for current goal");
				
				eosio::check(goal -> target.symbol == requested.symbol, "Requested symbol and goal symbol should equal");

				goals.modify(goal, creator, [&](auto &g){
		    	g.total_tasks = goal -> total_tasks + 1;
		    	g.target += requested;
		    });

			}

			//todo check calendar for 7 days

			tasks.emplace(creator, [&](auto &t){
				t.host = host;
				t.creator = creator;
				t.task_id = hash;
				t.goal_id = goal_id;
				t.title = title;
				t.data = data;
				t.priority = priority;
				t.permlink = permlink;
				t.requested = requested;
				t.for_each = for_each;
				t.funded = asset(0, root_symbol);
				t.remain = asset(0, root_symbol);
				t.is_public  = is_public;
				t.doer = doer;
				t.is_encrypted = false;
				t.validated = creator == acc->architect ? true : false;
				t.with_badge = with_badge;
				t.badge_id = badge_id;
				t.duration = duration;
				t.is_batch = is_batch;
				t.meta = meta;
				t.parent_batch_id = parent_batch_id;
				t.expired_at = eosio::time_point_sec (eosio::current_time_point().sec_since_epoch() + expiration);
				t.created_at = eosio::time_point_sec (eosio::current_time_point().sec_since_epoch());
				t.start_at = start_at;
				t.is_regular = is_regular;
				t.calendar = calendar;
			});
			
	    accounts.modify(acc, creator, [&](auto &a){
	      a.total_tasks = acc -> total_tasks + 1;
	    });


	    
		} else {

			tasks.modify(exist_task, creator, [&](auto &t){
				t.title = title;
				t.doer = doer;
				t.data = data;
				t.priority = priority;
				t.requested = requested;
				t.for_each = for_each;
				t.is_public = is_public;
				t.with_badge = with_badge;
				t.badge_id = badge_id;
				t.batch = batch;
				t.calendar = calendar;
				t.is_regular = is_regular;
				t.start_at = start_at;
			});


		}

		

		

		// uint64_t task_id = acc -> total_tasks + 1;
		
		

		
    
    // auto idx_bv = goals.template get_index<"hash"_n>();
    // auto exist_permlink = idx_bv.find(hash);

    // eosio::check(exist_task == tasks.end(), "Goal with current permlink is already exist");
    
    //TODO check batch for exist every task



		// if (creator == acc->architect) {
		// 	unicore::fundtask(host, task_id, requested, "fund on set by architect");
		// }

	}

	/**
	 * @brief      Публичный метод фондирования задачи
	 * Исполняется хостом для пополнения баланса задачи из доступного баланса цели. 
	 * 
	 * @param[in]  op    The operation
	 */
	[[eosio::action]] void unicore::fundtask(eosio::name host, uint64_t task_id, eosio::asset amount, eosio::string comment){
		require_auth(host);

		account_index accounts(_me, host.value);
		auto acc = accounts.find(host.value);
		eosio::check(acc != accounts.end(), "Host is not found");
		
		tasks_index tasks(_me, host.value);
		auto task = tasks.find(task_id);
		eosio::check(task != tasks.end(), "Task is not exist");
		goals_index goals(_me, host.value);
		
		auto goal = goals.find(task->goal_id);
		
		eosio::check(goal != goals.end(), "Goal is not found");

		eosio::check(goal->available >= amount, "Not enough tokens for fund in the goal object");

		eosio::check(task->funded + amount <= task->requested, "Fund amounts is more then requested");

		goals.modify(goal, host, [&](auto &g){
			g.available = goal->available - amount;
			g.withdrawed = goal->withdrawed + amount;
		});

		tasks.modify(task, host, [&](auto &t){
			t.funded = task->funded + amount;
			t.remain = task->remain + amount;
		});

	}

	/**
	 * @brief      Метод удаления задачи
	 * Вызывается хостом для удаления поставленной задачи. 
	 * 
	 * @param[in]  op    The operation
	 */
	[[eosio::action]] void unicore::deltask(eosio::name host, uint64_t task_id){
		require_auth(host);
		account_index accounts(_me, host.value);
		auto acc = accounts.find(host.value);
		eosio::check(acc != accounts.end(), "Host is not found ");

		tasks_index tasks(_me, host.value);

		auto task = tasks.find(task_id);
		eosio::check(task -> reports_count == 0, "Cannot delete task with reports. Delete all reports firstly");
		eosio::check(task != tasks.end(), "Task is not found");

		tasks.erase(task);

	}
	

	/**
	 * @brief      Метод активации задачи
	 * Вызывается хостом для активации выполнения поставленной задачи. 
	 * 
	 * @param[in]  op    The operation
	 */
	[[eosio::action]] void unicore::tactivate(eosio::name host, uint64_t task_id){
		require_auth(host);
		account_index accounts(_me, host.value);
		auto acc = accounts.find(host.value);
		eosio::check(acc != accounts.end(), "Host is not found");

		tasks_index tasks(_me, host.value);

		auto task = tasks.find(task_id);

		eosio::check(task != tasks.end(), "Task is not found");

		eosio::check(task->active == false, "Task is already active");

		tasks.modify(task, host, [&](auto &t){
			t.active = true;
		});

	}
	
	/**
	 * @brief      Публичный метод деактивации задачи
	 * Применимо для публичных задач, когда поставленная цель достигнута или недостижима.
	 * @param[in]  op    The operation
	 */
	[[eosio::action]] void unicore::tdeactivate(eosio::name host, uint64_t task_id){
		require_auth(host);
		account_index accounts(_me, host.value);
		auto acc = accounts.find(host.value);
		eosio::check(acc != accounts.end(), "Host is not found");

		tasks_index tasks(_me, host.value);

		auto task = tasks.find(task_id);

		eosio::check(task != tasks.end(), "Task is not found");

		eosio::check(task->active == true, "Task is already deactivated");

		tasks.modify(task, host, [&](auto &t){
			t.active = false;
		});

	}

	/**
	 * @brief      Публичный метод создания отчета о выполненной задаче
	 * Применяется исполнителем задачи для того, чтобы отправить отчет на проверку. 
	 * 
	 * @param[in]  op    The operation
	 */
	[[eosio::action]] void unicore::setreport(eosio::name host, eosio::name username, uint64_t task_id, eosio::string data){
		
		require_auth(username);
		account_index accounts(_me, host.value);
		partners_index users(_partners, _partners.value);


		auto acc = accounts.find(host.value);
		auto root_symbol = acc->get_root_symbol();
		
		// eosio::check(root_symbol == requested.symbol, "Wrong token for current host");
		eosio::check(acc != accounts.end(), "Host is not found");

		tasks_index tasks(_me, host.value);
		auto task = tasks.find(task_id);
		eosio::check(task != tasks.end(), "Task is not found");
		eosio::check(task -> active == true, "Task is not active");
		// eosio::check(task -> is_public == true, "Only public tasks is accessable for now");
	
		if (task -> is_public == false){
			eosio::check(task -> doer == username, "Wrong doer for the private task");
		};

		goals_index goals(_me, host.value);
		auto goal = goals.find(task-> goal_id);
		eosio::check(goal != goals.end(), "Goal is not found");

		if (goal -> type == "marathon"_n) {
			goalspartic_index gparticipants(_me, host.value);
      auto users_with_id = gparticipants.template get_index<"byusergoal"_n>();
			auto goal_ids = combine_ids(username.value, task->goal_id);
      auto participant = users_with_id.find(goal_ids);

      eosio::check(participant != users_with_id.end(), "Username not participant of the current marathon");
		};

		
		reports_index reports(_me, host.value);
		
		auto users_with_id = reports.template get_index<"userwithtask"_n>();

		auto report_ids = combine_ids(username.value, task_id);
		auto user_report = users_with_id.find(report_ids);

		// eosio::check(user_report == users_with_id.end(), "Report for this task already exist");
		
		bool need_check = username == host ? false : true;
		uint64_t report_id;


		if (user_report == users_with_id.end()){
			report_id = acc->total_reports + 1;
			reports.emplace(username, [&](auto &r) {
				r.report_id = report_id;
				r.task_id = task_id;
				r.goal_id = task->goal_id;
				r.count = 1;
				r.username = username;
				r.data = data;
				r.need_check = need_check;
				r.requested = task->for_each;
				r.balance = asset(0, root_symbol);
				r.withdrawed = asset(0, root_symbol);
				r.curator = host;
				r.expired_at = eosio::time_point_sec (eosio::current_time_point().sec_since_epoch() + 30 * 86400);
				r.created_at = eosio::time_point_sec (eosio::current_time_point().sec_since_epoch());
			});

			accounts.modify(acc, username, [&](auto &a){
	      a.total_reports = acc -> total_reports + 1;
	    });

			goals.modify(goal, username, [&](auto &g){
				g.reports_count += 1;
			});

	    tasks.modify(task, username, [&](auto &t){
				t.reports_count += 1;
			});

		} else {
			eosio::check(task -> is_regular == true, "Task is not regular, but report is exist");
			eosio::check(user_report -> need_check == false, "Previous report is not checked yet");
			report_id = user_report -> report_id;

			users_with_id.modify(user_report, username, [&](auto &r){
				r.count += 1;
				r.expired_at = eosio::time_point_sec (eosio::current_time_point().sec_since_epoch() + 30 * 86400);
				r.created_at = eosio::time_point_sec (eosio::current_time_point().sec_since_epoch());
				
			});
		}

    if (need_check == false) {
    	unicore::approver(host, report_id, "");
		};

		
	}

	/**
	 * @brief      Публиный метод редактирования отчета
	 * В случае, если отчет не принят, участник получает возможность отредактировать свой отчет и выслать его на проверку повторно. 
	 * @param[in]  op    The operation
	 */
	[[eosio::action]] void unicore::editreport(eosio::name host, eosio::name username, uint64_t report_id, eosio::string data){
		// require_auth(voter);
		require_auth(username);
		account_index accounts(_me, host.value);
		
		auto acc = accounts.find(host.value);
		eosio::check(acc != accounts.end(), "Host is not found");

		reports_index reports(_me, host.value);
		auto report = reports.find(report_id);

		eosio::check(report->need_check == false, "Cannot modify report until check");

		reports.modify(report, username, [&](auto &r){
			r.need_check = true;
			r.data = data;
			r.comment = "";
		});
	}

	/**
	 * @brief      Публичный метод одобрения отчета
	 * Используется хостом для того, чтобы принять задачу как выполненную и выдать вознаграждение / награду в виде значка. 
	 *
	 * @param[in]  op    The operation
	 */
	[[eosio::action]] void unicore::approver(eosio::name host, uint64_t report_id, eosio::string comment){
		require_auth(host);
		
		account_index accounts(_me, host.value);
		
		auto acc = accounts.find(host.value);
		eosio::check(acc != accounts.end(), "Host is not found");

		reports_index reports(_me, host.value);
		auto report = reports.find(report_id);
		
		tasks_index tasks(_me, host.value);
		auto task = tasks.find(report->task_id);
		
		if (report -> username != host)
			eosio::check(report->approved == false, "Task is already approved");

		reports.modify(report, host, [&](auto &r){
			r.need_check = false;
			r.comment = comment;
			r.approved = true;
			r.withdrawed = report->requested;
		});

		// eosio::check(report->requested <= task->remain, "Not enough funds for pay to this task");

		if (report->requested.amount > 0){
			if (report->requested > task->remain) {
				
				//if available in goal - minus it
				//or modify goal debt which will be payed on the next donation to the debt object
				
				goals_index goals(_me, host.value);
				auto goal = goals.find(task -> goal_id);
				eosio::check(goal != goals.end(), "Goal is not found");

				if (goal -> available >= report -> requested ){
					
					goals.modify(goal, host, [&](auto &g){
						g.available -= report -> requested;
						g.withdrawed += report -> requested;
					});

				} else {

					goals.modify(goal, host, [&](auto &g){
						g.available -= report -> requested;
						g.debt_count += 1;
					});
					
					debts_index debts(_me, _me.value);
					
					debts.emplace(host, [&](auto &d){
						d.id = goal->debt_count;
						d.amount = report -> requested;
						d.goal_id = goal->id;
						d.username = report -> username;
					});
				
				}

			} else {
				action(
          permission_level{ _me, "active"_n },
          acc->root_token_contract, "transfer"_n,
          std::make_tuple( _me, report->username, report->requested, comment) 
	      ).send();
				
		};

			tasks.modify(task, host, [&](auto &t){
				t.remain = task->remain - report->requested;
			});

	};

		if (task -> with_badge == true){
			unicore::giftbadge_action(host, report->username, task->badge_id, std::string("Completed task"), true, report->goal_id, report->task_id);
		}

    accounts.modify(acc, host, [&](auto &a){
      a.approved_reports = acc -> approved_reports + 1;
      // a.completed_tasks = task->remain.amount == 0 ? acc -> completed_tasks + 1 : acc -> completed_tasks;
    });
		
	}

	/**
	 * @brief      Публичный метод отклонения отчета
	 * 
	 * @param[in]  op    The operation
	 */
	[[eosio::action]] void unicore::disapprover(eosio::name host, uint64_t report_id, eosio::string comment){
		require_auth(host);
		account_index accounts(_me, host.value);
		
		auto acc = accounts.find(host.value);
		eosio::check(acc != accounts.end(), "Host is not found");

		reports_index reports(_me, host.value);
		auto report = reports.find(report_id);

		eosio::check(report->approved == false, "Report is already approved and cannot be disapproved now.");

		reports.modify(report, host, [&](auto &r){
			r.need_check = false;
			r.comment = comment;
		});

	};
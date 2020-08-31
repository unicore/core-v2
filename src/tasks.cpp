
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
	[[eosio::action]] void unicore::settask(eosio::name host, eosio::name username, uint64_t goal_id, eosio::string title, eosio::string data, eosio::asset requested, bool is_public, eosio::asset for_each, bool with_badge, uint64_t badge_id, uint64_t expiration){
		
		require_auth(username);
		account_index accounts(_me, host.value);
		auto acc = accounts.find(host.value);
		eosio::check(acc != accounts.end(), "Host is not found");


		auto root_symbol = acc->get_root_symbol();
		eosio::check(requested.symbol == root_symbol, "Wrong token for current host" );
		eosio::check(for_each.symbol == root_symbol, "Wrong token for current host" );

		goals_index goals(_me, host.value);
		auto goal = goals.find(goal_id);
		eosio::check(goal != goals.end(), "Goal is not found");

		tasks_index tasks(_me, host.value);
		
		uint64_t task_id = acc -> total_tasks + 1;
		
		badge_index badges(_me, host.value);

		if (with_badge == true){
			auto badge = badges.find(badge_id);
			eosio::check(badge != badges.end(), "Badge with current ID is not found");
		}
		
		
		tasks.emplace(username, [&](auto &t){
			t.creator = username;
			t.task_id = task_id;
			t.goal_id = goal_id;
			t.title = title;
			t.data = data;
			t.requested = requested;
			t.for_each = for_each;
			t.funded = asset(0, root_symbol);
			t.remain = asset(0, root_symbol);
			t.is_public  = is_public;
			t.validated = username == acc->architect ? true : false;
			t.with_badge = with_badge;
			t.badge_id = badge_id;
			t.expired_at = eosio::time_point_sec (eosio::current_time_point().sec_since_epoch() + expiration);
		});
		
    accounts.modify(acc, username, [&](auto &a){
      a.total_tasks = acc -> total_tasks + 1;
    });

    goals.modify(goal, username, [&](auto &g){
    	g.total_tasks = goal -> total_tasks + 1;
    });

		if (username == acc->architect) {

			unicore::fundtask(host, task_id, requested, "fund on set by architect");
		}
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
		auto user = users.find(username.value);
		eosio::check(user != users.end(), "User is not registered on the core");
		
		auto acc = accounts.find(host.value);
		auto root_symbol = acc->get_root_symbol();
		
		// eosio::check(root_symbol == requested.symbol, "Wrong token for current host");
		eosio::check(acc != accounts.end(), "Host is not found");

		tasks_index tasks(_me, host.value);
		auto task = tasks.find(task_id);
		eosio::check(task != tasks.end(), "Task is not found");
		eosio::check(task -> active == true, "Task is not active");
		eosio::check(task -> is_public == true, "Only public tasks is accessable for now");
	
		reports_index reports(_me, host.value);
		
		auto users_with_id = reports.template get_index<"userwithtask"_n>();

		auto report_ids = eosio::combine_ids(username.value, task_id);
		auto user_report = users_with_id.find(report_ids);

		eosio::check(user_report == users_with_id.end(), "Report for this task already exist");

		reports.emplace(username, [&](auto &r){
			r.report_id = acc->total_reports + 1;
			r.task_id = task_id;
			r.goal_id = task->goal_id;
			r.username = username;
			r.data = data;
			r.need_check = true;
			r.requested = task->for_each;
			r.balance = asset(0, root_symbol);
			r.withdrawed = asset(0, root_symbol);
			r.curator = host;
		});

    accounts.modify(acc, username, [&](auto &a){
      a.total_reports = acc -> total_reports + 1;
    });
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
	
		
		eosio::check(report->requested <= task->remain, "Not enough funds for pay to this task");

		eosio::check(report->approved == false, "Task is already approved");
		
		reports.modify(report, host, [&](auto &r){
			r.need_check = false;
			r.comment = comment;
			r.approved = true;
			r.withdrawed = report->requested;
		});


		if (report->requested.amount > 0){
				action(
		                permission_level{ _me, "active"_n },
		                acc->root_token_contract, "transfer"_n,
		                std::make_tuple( _me, report->username, report->requested, comment) 
	      ).send();
		};

		tasks.modify(task, host, [&](auto &t){
			t.remain = task->remain - report->requested;
		});


		if (task -> with_badge == true){
			unicore::giftbadge_action(host, report->username, task->badge_id, std::string("Completed task"));
		}


    accounts.modify(acc, host, [&](auto &a){
      a.approved_reports = acc -> approved_reports + 1;
      a.completed_tasks = task->remain.amount == 0 ? acc -> completed_tasks + 1 : acc -> completed_tasks;
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
namespace eosio{

/**
 * @brief      Модуль задач
 * Задачи есть составляющие части достижения любой цели. 
 * Постановка задач осуществляется только в рамках целей. 
 * Задачи могут быть 
 * 	- публичными, т.е. достуными к исполнению каждым участником сообщества, 
 * 	- приватными, т.е. доступными к исполнению только внутренним управляющим командам.
 */

struct tsks
{
	/**
	 * @brief      Публичный метод создания задачи
	 * Может использовать только аккаунт хоста на текущий момент. 
	 *
	 * @param[in]  op    The operation
	 */
	void settask_action (const settask &op){
		
		require_auth(op.username);
		account_index accounts(_self, op.host.value);
		auto acc = accounts.find(op.host.value);
		check(acc != accounts.end(), "Host is not found");


		auto root_symbol = acc->get_root_symbol();
		check(op.requested.symbol == root_symbol, "Wrong token for current host" );
		check(op.for_each.symbol == root_symbol, "Wrong token for current host" );

		goals_index goals(_self, op.host.value);
		auto goal = goals.find(op.goal_id);
		check(goal != goals.end(), "Goal is not found");

		tasks_index tasks(_self, op.host.value);
		
		uint64_t task_id = tasks.available_primary_key();
		
		tasks.emplace(op.host, [&](auto &t){
			t.task_id = task_id;
			t.goal_id = op.goal_id;
			t.title = op.title;
			t.data = op.data;
			t.requested = op.requested;
			t.for_each = op.for_each;
			t.funded = asset(0, root_symbol);
			t.remain = asset(0, root_symbol);
			t.is_public  = op.is_public;
			t.with_badge = op.with_badge;
			t.badge_id = op.badge_id;
			t.expired_at = eosio::time_point_sec (eosio::current_time_point().sec_since_epoch() + op.expiration);
		});
		
		if (op.username == acc->architect){
			fundtask fop;
			fop.host = op.host;
			fop.task_id = task_id;
			fop.amount = op.requested;
			fop.comment = "fund on set by architect";
			fundtask_action(fop);
		}
	}

	/**
	 * @brief      Публичный метод фондирования задачи
	 * Исполняется хостом для пополнения баланса задачи из доступного баланса цели. 
	 * 
	 * @param[in]  op    The operation
	 */
	void fundtask_action(const fundtask &op){
		require_auth(op.host);

		account_index accounts(_self, op.host.value);
		auto acc = accounts.find(op.host.value);
		check(acc != accounts.end(), "Host is not found");
		
		tasks_index tasks(_self, op.host.value);
		auto task = tasks.find(op.task_id);
		check(task != tasks.end(), "Task is not exist");
		goals_index goals(_self, op.host.value);
		
		auto goal = goals.find(task->goal_id);
		
		check(goal != goals.end(), "Goal is not found");

		check(goal->available >= op.amount, "Not enough tokens for fund in the goal object");

		check(task->funded + op.amount <= task->requested, "Fund amounts is more then requested");

		goals.modify(goal, op.host, [&](auto &g){
			g.available = goal->available - op.amount;
			g.withdrawed = goal->withdrawed + op.amount;
		});

		tasks.modify(task, op.host, [&](auto &t){
			t.funded = task->funded + op.amount;
			t.remain = task->remain + op.amount;
		});

	}

	/**
	 * @brief      Метод активации задачи
	 * Вызывается хостом для активации выполнения поставленной задачи. 
	 * 
	 * @param[in]  op    The operation
	 */
	void tactivate_action (const tactivate &op){
		require_auth(op.host);
		account_index accounts(_self, op.host.value);
		auto acc = accounts.find(op.host.value);
		check(acc != accounts.end(), "Host is not found");

		tasks_index tasks(_self, op.host.value);

		auto task = tasks.find(op.task_id);

		check(task != tasks.end(), "Task is not found");

		check(task->active == false, "Task is already active");

		tasks.modify(task, op.host, [&](auto &t){
			t.active = true;
		});

	}
	
	/**
	 * @brief      Публичный метод деактивации задачи
	 * Применимо для публичных задач, когда поставленная цель достигнута или недостижима.
	 * @param[in]  op    The operation
	 */
	void tdeactivate_action (const tdeactivate &op){
		require_auth(op.host);
		account_index accounts(_self, op.host.value);
		auto acc = accounts.find(op.host.value);
		check(acc != accounts.end(), "Host is not found");

		tasks_index tasks(_self, op.host.value);

		auto task = tasks.find(op.task_id);

		check(task != tasks.end(), "Task is not found");

		check(task->active == true, "Task is already deactivated");

		tasks.modify(task, op.host, [&](auto &t){
			t.active = false;
		});

	}

	/**
	 * @brief      Публичный метод создания отчета о выполненной задаче
	 * Применяется исполнителем задачи для того, чтобы отправить отчет на проверку. 
	 * 
	 * @param[in]  op    The operation
	 */
	void setreport_action (const setreport &op){
		// eosio::name host;
		// eosio::name username;
		// uint64_t task_id;
		// eosio::string data;

		require_auth(op.username);
		account_index accounts(_self, op.host.value);
		user_index users(_self, _self.value);
		auto user = users.find(op.username.value);
		check(user != users.end(), "User is not registered on the core");
		
		auto acc = accounts.find(op.host.value);
		auto root_symbol = acc->get_root_symbol();
		
		// check(root_symbol == op.requested.symbol, "Wrong token for current host");
		check(acc != accounts.end(), "Host is not found");

		tasks_index tasks(_self, op.host.value);
		auto task = tasks.find(op.task_id);
		check(task != tasks.end(), "Task is not found");
		check(task -> active == true, "Task is not active");
		check(task -> is_public == true, "Only public tasks is accessable for now");
	
		reports_index reports(_self, op.host.value);
		
		auto users_with_id = reports.template get_index<"userwithtask"_n>();

		auto report_ids = combine_ids(op.username.value, op.task_id);
		auto user_report = users_with_id.find(report_ids);

		check(user_report == users_with_id.end(), "Report for this task already exist");

		reports.emplace(op.username, [&](auto &r){
			r.report_id = reports.available_primary_key();
			r.task_id = op.task_id;
			r.goal_id = task->goal_id;
			r.username = op.username;
			r.data = op.data;
			r.need_check = true;
			r.requested = task->for_each;
			r.balance = asset(0, root_symbol);
			r.withdrawed = asset(0, root_symbol);
			r.curator = op.host;
		});

	}

	/**
	 * @brief      Публиный метод редактирования отчета
	 * В случае, если отчет не принят, участник получает возможность отредактировать свой отчет и выслать его на проверку повторно. 
	 * @param[in]  op    The operation
	 */
	void editreport_action (const editreport &op){
		// require_auth(op.voter);
		require_auth(op.username);
		account_index accounts(_self, op.host.value);
		
		auto acc = accounts.find(op.host.value);
		check(acc != accounts.end(), "Host is not found");

		reports_index reports(_self, op.host.value);
		auto report = reports.find(op.report_id);

		check(report->need_check == false, "Cannot modify report until check");

		reports.modify(report, op.username, [&](auto &r){
			r.need_check = true;
			r.data = op.data;
			r.comment = "";
		});
	}

	/**
	 * @brief      Публичный метод одобрения отчета
	 * Используется хостом для того, чтобы принять задачу как выполненную и выдать вознаграждение / награду в виде значка. 
	 *
	 * @param[in]  op    The operation
	 */
	void approver_action (const approver &op){
		// require_auth(op.voter);
		require_auth(op.host);
		
		account_index accounts(_self, op.host.value);
		
		auto acc = accounts.find(op.host.value);
		check(acc != accounts.end(), "Host is not found");

		reports_index reports(_self, op.host.value);
		auto report = reports.find(op.report_id);
		
		tasks_index tasks(_self, op.host.value);
		auto task = tasks.find(report->task_id);
	
		
		check(report->requested <= task->remain, "Not enough funds for pay to this task");

		check(report->approved == false, "Task is already approved");
		
		reports.modify(report, op.host, [&](auto &r){
			r.need_check = false;
			r.comment = op.comment;
			r.approved = true;
			r.withdrawed = report->requested;
		});


		if (report->requested.amount > 0){
			action(
	                permission_level{ _self, "active"_n },
	                acc->root_token_contract, "transfer"_n,
	                std::make_tuple( _self, report->username, report->requested, op.comment) 
	        ).send();
		};

		tasks.modify(task, op.host, [&](auto &t){
			t.remain = task->remain - report->requested;
		});


		if (task -> with_badge == true){
			giftbadge gift;
			gift.host = op.host;
			gift.to = report->username;
			gift.badge_id = task->badge_id;
			gift.comment = std::string("Completed task");

			badge_struct().giftbadge_action(gift);
		}

	
	}

	/**
	 * @brief      Публичный метод отклонения отчета
	 * 
	 * @param[in]  op    The operation
	 */
	void disapprover_action (const disapprover &op){
		require_auth(op.host);
		account_index accounts(_self, op.host.value);
		
		auto acc = accounts.find(op.host.value);
		check(acc != accounts.end(), "Host is not found");

		reports_index reports(_self, op.host.value);
		auto report = reports.find(op.report_id);

		check(report->approved == false, "Report is already approved and cannot be disapproved now.");

		reports.modify(report, op.host, [&](auto &r){
			r.need_check = false;
			r.comment = op.comment;
		});

	}
		

};
}
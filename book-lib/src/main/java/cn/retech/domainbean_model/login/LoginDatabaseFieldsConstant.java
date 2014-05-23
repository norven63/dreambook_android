package cn.retech.domainbean_model.login;

public final class LoginDatabaseFieldsConstant {
	private LoginDatabaseFieldsConstant() {

	}

	public static enum RequestBean {
		// 登录名
		user_id,
		// 密码
		user_password
	}

	public static enum RespondBean {
		//
		response,
		//
		validate,
		//
		error, 
		user, // 登录用户名
		password, // 登录密码
		companyName, // 公司名字
		subscriptionCode, // 隶属公司ID
		name, // 用户名
		nameKana, // 昵称
		email// 邮箱
	}
}

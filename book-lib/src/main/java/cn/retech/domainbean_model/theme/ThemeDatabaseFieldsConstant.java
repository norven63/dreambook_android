package cn.retech.domainbean_model.theme;

public final class ThemeDatabaseFieldsConstant {
	private ThemeDatabaseFieldsConstant() {

	}

	public static enum RequestBean {
		// 登录名
		user_id,
		// 密码
		user_password,
		// 主题包平台(1 : iOS , 2: Android)
		os
	}

	public static enum RespondBean {
		//
		theme,
		// 校验
		validate,
		// 公司名称
		name,
		//
		version,
		//
		url,
		// 公司id
		companyid;
	}
}

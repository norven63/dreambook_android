package cn.retech.domainbean_model.theme;

import java.util.HashMap;
import java.util.Map;

import android.text.TextUtils;
import cn.retech.my_network_engine.domainbean_tools.IParseDomainBeanToDataDictionary;

public final class ThemeParseDomainBeanToDD implements IParseDomainBeanToDataDictionary {

	@Override
	public Map<String, String> parseDomainBeanToDataDictionary(Object netRequestDomainBean) {

		if (null == netRequestDomainBean) {
			throw new IllegalArgumentException("netRequestDomainBean is null!");
		}

		boolean isRightObjectType = netRequestDomainBean instanceof ThemeNetRequestBean;
		if (!isRightObjectType) {
			throw new IllegalArgumentException("传入的业务Bean的类型不符 !");
		}

		ThemeNetRequestBean requestBean = (ThemeNetRequestBean) netRequestDomainBean;
		String username = requestBean.getUsername();
		String password = requestBean.getPassword();
		if (TextUtils.isEmpty(username) || TextUtils.isEmpty(password)) {
			throw new IllegalArgumentException("必须的数据字段不完整 ! ");
		}

		Map<String, String> params = new HashMap<String, String>();
		// 必须参数
		params.put(ThemeDatabaseFieldsConstant.RequestBean.user_id.name(), username);
		params.put(ThemeDatabaseFieldsConstant.RequestBean.user_password.name(), password);
		// 主题包平台类型(1 : iOS, 2 : Android)
		params.put(ThemeDatabaseFieldsConstant.RequestBean.os.name(), "2");
		return params;
	}
}

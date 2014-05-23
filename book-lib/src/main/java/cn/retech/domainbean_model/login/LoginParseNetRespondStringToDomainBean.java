package cn.retech.domainbean_model.login;

import java.io.ByteArrayInputStream;
import java.io.InputStream;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserFactory;

import cn.retech.my_network_engine.domainbean_tools.IParseNetRespondDataToDomainBean;

public final class LoginParseNetRespondStringToDomainBean implements IParseNetRespondDataToDomainBean {

	@Override
	public Object parseNetRespondDataToDomainBean(final Object netRespondData) throws Exception {

		final String netRespondString = (String) netRespondData;

		InputStream inputStreamOfRespondData = new ByteArrayInputStream(netRespondString.getBytes("UTF-8"));

		XmlPullParserFactory parserCreator = XmlPullParserFactory.newInstance();
		XmlPullParser parser = parserCreator.newPullParser();
		parser.setInput(inputStreamOfRespondData, null);
		String user = "";// 登录用户名
		@SuppressWarnings("unused")
    String password = "";// 登录密码
		String companyName = "";// 公司名字
		String subscriptionCode = "";// 隶属公司ID
		String name = "";// 用户名
		String nameKana = "";// 昵称
		String email = "";// 邮箱

		int parserEvent = parser.getEventType();
		while (parserEvent != XmlPullParser.END_DOCUMENT) {

			switch (parserEvent) {
			case XmlPullParser.START_TAG:// 开始元素事件
				if (parser.getName().equalsIgnoreCase(LoginDatabaseFieldsConstant.RespondBean.validate.name())) {// 登录用户名
					user = parser.nextText();
				} else if (parser.getName().equalsIgnoreCase(LoginDatabaseFieldsConstant.RespondBean.companyName.name())) {// 公司名字
					companyName = parser.nextText();
				} else if (parser.getName().equalsIgnoreCase(LoginDatabaseFieldsConstant.RespondBean.subscriptionCode.name())) {// 隶属公司ID
					subscriptionCode = parser.nextText();
				} else if (parser.getName().equalsIgnoreCase(LoginDatabaseFieldsConstant.RespondBean.name.name())) {// 用户名
					name = parser.nextText();
				} else if (parser.getName().equalsIgnoreCase(LoginDatabaseFieldsConstant.RespondBean.nameKana.name())) {// 昵称
					nameKana = parser.nextText();
				} else if (parser.getName().equalsIgnoreCase(LoginDatabaseFieldsConstant.RespondBean.email.name())) {// 邮箱
					email = parser.nextText();
				}
				break;

			case XmlPullParser.END_TAG:// 结束元素事件
				break;
			}

			parserEvent = parser.next();
		}
		inputStreamOfRespondData.close();
		LoginNetRespondBean logonNetRespondBean = new LoginNetRespondBean();
		logonNetRespondBean.setCompanyName(companyName);
		logonNetRespondBean.setEmail(email);
		logonNetRespondBean.setName(name);
		logonNetRespondBean.setNameKana(nameKana);
		logonNetRespondBean.setSubscriptionCode(subscriptionCode);
		logonNetRespondBean.setUser(user);
		return logonNetRespondBean;
	}
}

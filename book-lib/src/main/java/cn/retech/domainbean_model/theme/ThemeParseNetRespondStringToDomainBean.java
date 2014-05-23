package cn.retech.domainbean_model.theme;

import java.io.ByteArrayInputStream;
import java.io.InputStream;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserFactory;

import cn.retech.my_network_engine.domainbean_tools.IParseNetRespondDataToDomainBean;

public final class ThemeParseNetRespondStringToDomainBean implements IParseNetRespondDataToDomainBean {

	@Override
	public Object parseNetRespondDataToDomainBean(final Object netRespondData) throws Exception {

		final String netRespondString = (String) netRespondData;

		InputStream inputStreamOfRespondData = new ByteArrayInputStream(netRespondString.getBytes("UTF-8"));

		XmlPullParserFactory parserCreator = XmlPullParserFactory.newInstance();
		XmlPullParser parser = parserCreator.newPullParser();
		parser.setInput(inputStreamOfRespondData, null);

		// 是否有效
		boolean validate = false;
		// 账号名称
		String name = "";
		// 版本
		String version = "";
		// 主题包下载url
		String url = "";
		// 公司id
		String companyid = "";

		int parserEvent = parser.getEventType();
		while (parserEvent != XmlPullParser.END_DOCUMENT) {

			switch (parserEvent) {
			case XmlPullParser.START_TAG:// 开始元素事件
				if (parser.getName().equalsIgnoreCase(ThemeDatabaseFieldsConstant.RespondBean.validate.name())) {
					validate = Boolean.parseBoolean(parser.nextText());
				} else if (parser.getName().equalsIgnoreCase(ThemeDatabaseFieldsConstant.RespondBean.name.name())) {
					name = parser.nextText();
				} else if (parser.getName().equalsIgnoreCase(ThemeDatabaseFieldsConstant.RespondBean.version.name())) {
					version = parser.nextText();
				} else if (parser.getName().equalsIgnoreCase(ThemeDatabaseFieldsConstant.RespondBean.url.name())) {
					url = parser.nextText();
				} else if (parser.getName().equalsIgnoreCase(ThemeDatabaseFieldsConstant.RespondBean.companyid.name())) {
					companyid = parser.nextText();
				}
				break;

			case XmlPullParser.END_TAG:// 结束元素事件
				break;
			}

			parserEvent = parser.next();
		}
		inputStreamOfRespondData.close();
		return new ThemeNetRespondBean(name, version, url, companyid, validate);
	}
}

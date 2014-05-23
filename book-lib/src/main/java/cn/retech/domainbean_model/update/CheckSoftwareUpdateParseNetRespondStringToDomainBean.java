package cn.retech.domainbean_model.update;

import java.io.ByteArrayInputStream;
import java.io.InputStream;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserFactory;

import cn.retech.my_network_engine.domainbean_tools.IParseNetRespondDataToDomainBean;

public class CheckSoftwareUpdateParseNetRespondStringToDomainBean implements IParseNetRespondDataToDomainBean {

	@Override
	public Object parseNetRespondDataToDomainBean(final Object netRespondData) throws Exception {
		final String netRespondString = (String) netRespondData;

		InputStream inputStreamOfRespondData = new ByteArrayInputStream(netRespondString.getBytes("UTF-8"));

		XmlPullParserFactory parserCreator = XmlPullParserFactory.newInstance();
		XmlPullParser parser = parserCreator.newPullParser();
		parser.setInput(inputStreamOfRespondData, null);

		// 渠道名
		String channelName = "";
		// 是否升级
		boolean isUpdate = false;
		// 软件版本号
		String version = "";
		// 下载地址
		String trackViewUrl = "";
		// 备注
		String notes = "";
		// 更新信息
		String releaseNotes = "";
		// 文件大小
		int fileSizeBytes = 0;
		// MD5
		String mD5String = "";

		int parserEvent = parser.getEventType();
		while (parserEvent != XmlPullParser.END_DOCUMENT) {

			switch (parserEvent) {
			case XmlPullParser.START_TAG:// 开始元素事件
				if (parser.getName().equalsIgnoreCase("channel")) {
					channelName = parser.getAttributeValue(null, CheckSoftwareUpdateFieldsConstant.RespondBean.name.name());
				} else if (parser.getName().equalsIgnoreCase(CheckSoftwareUpdateFieldsConstant.RespondBean.updata.name())) {
					isUpdate = new Boolean(parser.nextText()).booleanValue();
				} else if (parser.getName().equalsIgnoreCase(CheckSoftwareUpdateFieldsConstant.RespondBean.version.name())) {
					version = parser.nextText();
				} else if (parser.getName().equalsIgnoreCase(CheckSoftwareUpdateFieldsConstant.RespondBean.trackViewUrl.name())) {
					trackViewUrl = parser.nextText();
				} else if (parser.getName().equalsIgnoreCase(CheckSoftwareUpdateFieldsConstant.RespondBean.notes.name())) {
					notes = parser.nextText();
				} else if (parser.getName().equalsIgnoreCase(CheckSoftwareUpdateFieldsConstant.RespondBean.releaseNotes.name())) {
					releaseNotes = parser.nextText();
				} else if (parser.getName().equalsIgnoreCase(CheckSoftwareUpdateFieldsConstant.RespondBean.fileSizeBytes.name())) {
					// fileSizeBytes = Integer.parseInt(parser.nextText());
					fileSizeBytes = 500;
				} else if (parser.getName().equalsIgnoreCase(CheckSoftwareUpdateFieldsConstant.RespondBean.MD5.name())) {
					mD5String = parser.nextText();
				}
				break;

			case XmlPullParser.END_TAG:// 结束元素事件
				break;
			}

			parserEvent = parser.next();
		}
		inputStreamOfRespondData.close();

		return new CheckSoftwareUpdateRespondBean(channelName, isUpdate, version, trackViewUrl, notes, releaseNotes, fileSizeBytes, mD5String);
	}
}

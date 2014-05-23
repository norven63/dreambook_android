package cn.retech.domainbean_model.update;

public class CheckSoftwareUpdateFieldsConstant {
	public static enum RequestBean {
	}

	public static enum RespondBean {
		// 渠道名
		name,
		// 是否升级
		updata,
		// 软件版本号
		version,
		// 下载地址
		trackViewUrl,
		// 备注
		notes,
		// 更新信息
		releaseNotes,
		// 文件大小
		fileSizeBytes,
		// MD5
		MD5,

	}
}

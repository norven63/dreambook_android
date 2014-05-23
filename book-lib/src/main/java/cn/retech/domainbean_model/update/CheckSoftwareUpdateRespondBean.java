package cn.retech.domainbean_model.update;

public class CheckSoftwareUpdateRespondBean {
	// 渠道名
	private String channelName;
	// 是否升级
	private boolean isUpdate;
	// 软件版本号
	private String version;
	// 下载地址
	private String trackViewUrl;
	// 备注
	private String notes;
	// 更新信息
	private String releaseNotes;
	// 文件大小
	private int fileSizeBytes;
	// MD5
	private String mD5String;

	public CheckSoftwareUpdateRespondBean(String channelName, boolean isUpdate, String version, String trackViewUrl, String notes, String releaseNotes, int fileSizeBytes, String mD5String) {
		super();
		this.channelName = channelName;
		this.isUpdate = isUpdate;
		this.version = version;
		this.trackViewUrl = trackViewUrl;
		this.notes = notes;
		this.releaseNotes = releaseNotes;
		this.fileSizeBytes = fileSizeBytes;
		this.mD5String = mD5String;
	}

	public String getChannelName() {
		return channelName;
	}

	public boolean isUpdate() {
		return isUpdate;
	}

	public String getVersion() {
		return version;
	}

	public String getTrackViewUrl() {
		return trackViewUrl;
	}

	public String getNotes() {
		return notes;
	}

	public String getReleaseNotes() {
		return releaseNotes;
	}

	public int getFileSizeBytes() {
		return fileSizeBytes;
	}

	public String getmD5String() {
		return mD5String;
	}

	@Override
	public String toString() {
		return "CheckSoftwareUpdateRespondBean [channelName=" + channelName + ", isUpdate=" + isUpdate + ", version=" + version + ", trackViewUrl=" + trackViewUrl + ", notes=" + notes
				+ ", releaseNotes=" + releaseNotes + ", fileSizeByte=" + fileSizeBytes + ", mD5String=" + mD5String + "]";
	}

}

package cn.retech.domainbean_model.theme;

public final class ThemeNetRespondBean {
	private String name;
	private String version;
	private String url;
	private String companyid;
	// 是否有主题包, 如果没有的话, 就使用app自带的默认主题包
	private boolean validate;

	public ThemeNetRespondBean(String name, String version, String url, String companyid, boolean validate) {
		super();
		this.name = name;
		this.version = version;
		this.url = url;
		this.companyid = companyid;
		this.validate = validate;
	}

	public String getName() {
		return name;
	}

	public String getVersion() {
		return version;
	}

	public String getUrl() {
		return url;
	}

	public String getCompanyid() {
		return companyid;
	}

	public boolean isValidate() {
		return validate;
	}

	@Override
	public String toString() {
		return "ThemeNetRespondBean [name=" + name + ", version=" + version + ", url=" + url + ", companyid=" + companyid + ", validate=" + validate + "]";
	}

}

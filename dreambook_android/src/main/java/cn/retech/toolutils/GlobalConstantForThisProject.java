package cn.retech.toolutils;

import static com.google.common.collect.Maps.newHashMap;

import java.util.HashMap;

import cn.retech.activity.R;

import com.google.common.collect.ImmutableMap;

public class GlobalConstantForThisProject {
	// 创建价格图片的id集合
	public final static ImmutableMap<String, Integer> priceImageMap;

	static {

		HashMap<String, Integer> priceMap = newHashMap();
		priceMap.put("0.0", R.drawable.price_free);
		priceMap.put("118.0", R.drawable.price_118);
		priceMap.put("108.0", R.drawable.price_108);
		priceMap.put("12.0", R.drawable.price_12);
		priceMap.put("113.0", R.drawable.price_113);
		priceMap.put("123.0", R.drawable.price_123);
		priceMap.put("128.0", R.drawable.price_128);
		priceMap.put("138.0", R.drawable.price_138);
		priceMap.put("148.0", R.drawable.price_148);
		priceMap.put("153.0", R.drawable.price_153);
		priceMap.put("158.0", R.drawable.price_158);
		priceMap.put("163.0", R.drawable.price_163);
		priceMap.put("168.0", R.drawable.price_168);
		priceMap.put("178.0", R.drawable.price_178);
		priceMap.put("18.0", R.drawable.price_18);
		priceMap.put("188.0", R.drawable.price_188);
		priceMap.put("193.0", R.drawable.price_193);
		priceMap.put("198.0", R.drawable.price_198);
		priceMap.put("208.0", R.drawable.price_208);
		priceMap.put("25.0", R.drawable.price_25);
		priceMap.put("30.0", R.drawable.price_30);
		priceMap.put("40.0", R.drawable.price_40);
		priceMap.put("45.0", R.drawable.price_45);
		priceMap.put("50.0", R.drawable.price_50);
		priceMap.put("6.0", R.drawable.price_6);
		priceMap.put("60.0", R.drawable.price_60);
		priceMap.put("68.0", R.drawable.price_68);
		priceMap.put("73.0", R.drawable.price_73);
		priceMap.put("78.0", R.drawable.price_78);
		priceMap.put("88.0", R.drawable.price_88);
		priceMap.put("93.0", R.drawable.price_93);
		priceMap.put("98.0", R.drawable.price_98);

		priceImageMap = ImmutableMap.copyOf(priceMap);
	}

	// 公共账号
	public final static String PUBLIC_ACCOUNT_USERNAME = "public";
	public final static String PUBLIC_ACCOUNT_PASSWORD = "pwpublic";
}

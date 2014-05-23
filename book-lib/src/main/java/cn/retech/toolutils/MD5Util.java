package cn.retech.toolutils;

import java.io.FileInputStream;
import java.io.InputStream;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

public class MD5Util {
	private static final char HEX_DIGITS[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	private static MessageDigest messageDigest = null;

	static {
		try {
			messageDigest = MessageDigest.getInstance("MD5");
		} catch (NoSuchAlgorithmException e) {
			e.printStackTrace();
		}
	}

	public static String getMD5String(String str) {
		return getMD5String(str.getBytes());
	}

	public static String getMD5String(byte[] bytes) {
		messageDigest.update(bytes);
		return bytesToHex(messageDigest.digest());
	}

	public static String bytesToHex(byte bytes[]) {
		return bytesToHex(bytes, 0, bytes.length);
	}

	public static String bytesToHex(byte bytes[], int start, int end) {
		StringBuilder sb = new StringBuilder();

		for (int i = start; i < start + end; i++) {
			sb.append(byteToHex(bytes[i]));
		}

		return sb.toString();
	}

	public static String byteToHex(byte bt) {
		return HEX_DIGITS[(bt & 0xf0) >> 4] + "" + HEX_DIGITS[bt & 0xf];
	}

	/**
	 * 将文件使用MD5加密
	 * 
	 * @param filename
	 * @return
	 */
	public static String md5sum(String filename) {
		InputStream fis;
		byte[] buffer = new byte[1024];
		int numRead = 0;
		MessageDigest md5;
		try {
			fis = new FileInputStream(filename);
			md5 = MessageDigest.getInstance("MD5");
			while ((numRead = fis.read(buffer)) > 0) {
				md5.update(buffer, 0, numRead);
			}
			fis.close();
			return bytesToHex(md5.digest());
		} catch (Exception e) {
			System.out.println("error:" + e.toString());
			return null;
		}

	}
}

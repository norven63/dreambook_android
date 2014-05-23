package cn.retech.domainbean_model.book;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

import android.text.TextUtils;

public class BookFile implements Serializable {
	/**
   * 
   */
	private static final long serialVersionUID = -2775454475801491883L;

	private BookFile() {

	}

	// 私有属性
	private boolean isFile;

	// file 相关的方法
	private String contentID;

	public String getContentID() {
		return contentID;
	}

	// directory 相关的方法

	private String directoryName;

	public void setDirectoryName(String directoryName) {
		this.directoryName = directoryName;
	}

	public String getDirectoryName() {
		return directoryName;
	}

	private List<BookFile> listFiles;

	public List<BookFile> getListFiles() {
		return listFiles;
	}

	// 用于判断是文件还是文件夹
	public boolean isFile() {
		return isFile;
	}

	public boolean isDirectory() {
		return !isFile;
	}

	// 用于创建文件/文件夹
	public static BookFile createFileWithContentID(String contentID) {
		if (TextUtils.isEmpty(contentID)) {
			assert false : "入参不合法";
			return null;
		}
		BookFile file = new BookFile();
		file.isFile = true;
		file.contentID = contentID;
		file.listFiles = null;
		return file;
	}

	public static BookFile createDirectoryWithName(String directoryName, List<BookFile> files) {
		if (TextUtils.isEmpty(directoryName) || files == null) {
			assert false : "入参不合法";
			return null;
		}

		BookFile directory = new BookFile();
		directory.isFile = false;
		directory.directoryName = directoryName;
		directory.listFiles = new ArrayList<BookFile>();
		directory.listFiles.addAll(files);
		return directory;
	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		if (isFile) {
			result = prime * result + ((contentID == null) ? 0 : contentID.hashCode());
		} else {
			result = prime * result + ((directoryName == null) ? 0 : directoryName.hashCode());
		}
		return result;
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		BookFile other = (BookFile) obj;
		if (isFile) {
			if (contentID == null) {
				if (other.contentID != null)
					return false;
			} else if (!contentID.equals(other.contentID))
				return false;
		} else {
			if (directoryName == null) {
				if (other.directoryName != null)
					return false;
			} else if (!directoryName.equals(other.directoryName))
				return false;
		}

		return true;
	}

	// 在当前文件夹目录下面, 创建一个有效的新文件夹名称(所谓有效的新文件夹名称就是在当前文件夹目录下面, 不能有重名的子文件夹存在)
	public String createValidNewDirectoryNameInCurrentlyDirectory() {
		if (isFile) {
			assert false : "文件不具备此方法, createValidNewDirectoryNameInCurrentlyDirectory";
			return null;
		}

		int i = 1;
		String newDirectoryName = null;
		while (true) {
			boolean isValidDirectoryName = true;
			newDirectoryName = "新文件夹 " + i++;
			for (BookFile file : listFiles) {
				if (file.isFile) {
					continue;
				}

				if (file.directoryName.equals(newDirectoryName)) {
					isValidDirectoryName = false;
					break;
				}
			}

			if (isValidDirectoryName) {
				break;
			}
		}

		return newDirectoryName;
	}

	// 更改文件夹名称, 如果成功返回 YES, 否则返回 NO
	public boolean changeDirectoryNameWithNewName(String newDirectoryName) {
		boolean isValidDirectoryName = true;
		for (BookFile file : listFiles) {
			if (file.isFile) {
				continue;
			}

			if (file.directoryName.equals(newDirectoryName)) {
				isValidDirectoryName = false;
				break;
			}
		}

		if (isValidDirectoryName) {
			this.directoryName = newDirectoryName;
		}

		return isValidDirectoryName;

	}

	// 单独文件总数(不包括文件夹)
	public int separateFilesTotal() {
		if (isFile) {
			return 1;
		} else {
			int total = 0;
			for (BookFile file : listFiles) {
				total += file.separateFilesTotal();
			}
			return total;
		}
	}

	// 获取所有的单独文件
	public List<BookFile> allSeparateFiles() {
		if (isFile) {
			List<BookFile> list = new ArrayList<BookFile>(1);
			list.add(this);
			return list;
		} else {
			List<BookFile> separateFiles = new ArrayList<BookFile>();
			for (BookFile file : listFiles) {
				separateFiles.addAll(file.allSeparateFiles());
			}
			return separateFiles;
		}
	}

	// 获取所有已经包含的contentID
	public List<String> allContentID() {
		List<BookFile> allSeparateFiles = allSeparateFiles();
		List<String> allContentID = new ArrayList<String>(allSeparateFiles.size());
		for (BookFile separateFile : allSeparateFiles) {
			allContentID.add(separateFile.contentID);
		}
		return allContentID;
	}
}

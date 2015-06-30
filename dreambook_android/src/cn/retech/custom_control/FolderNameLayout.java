package cn.retech.custom_control;

import android.content.Context;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import cn.retech.activity.R;

public class FolderNameLayout extends RelativeLayout {
	private ImageView leftView;
	private EditText middleView;
	private ImageButton rightView;

	public FolderNameLayout(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		init(context, attrs);
	}

	public FolderNameLayout(Context context, AttributeSet attrs) {
		super(context, attrs);
		init(context, attrs);
	}

	public FolderNameLayout(Context context) {
		super(context);
		init(context, null);
	}

	private void init(Context context, AttributeSet attrs) {
		LayoutInflater layoutInflater = (LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		layoutInflater.inflate(R.layout.folder_name_layout, this);

		leftView = (ImageView) findViewById(R.id.leftView);
		middleView = (EditText) findViewById(R.id.middleView);
		rightView = (ImageButton) findViewById(R.id.rightView);
	}

	public EditText getNameEditText() {
		return middleView;
	}

	public ImageButton getCancelButton() {
		return rightView;
	}

	public void setLeftViewImage(Drawable leftViewImage) {
		leftView.setBackgroundDrawable(leftViewImage);
	}

	public void setMiddleViewImage(Drawable middleViewImage) {
		middleView.setBackgroundDrawable(middleViewImage);
	}

	public void setRightViewImage(Drawable rightViewImage) {
		rightView.setBackgroundDrawable(rightViewImage);
	}
}

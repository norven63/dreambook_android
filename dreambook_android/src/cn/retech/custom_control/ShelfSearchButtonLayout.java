package cn.retech.custom_control;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import cn.retech.activity.R;

public class ShelfSearchButtonLayout extends RelativeLayout {
	private ImageView leftView;
	private ImageView middleView;
	private ImageView rightView;

	private int leftViewImageId;
	private int middleViewImageId;
	private int rightViewImageId;
	private int leftViewImagePressedId;
	private int middleViewImagePressedId;
	private int rightViewImagePressedId;

	private Drawable leftViewImage;
	private Drawable middleViewImage;
	private Drawable rightViewImage;
	private Drawable leftViewImagePressed;
	private Drawable middleViewImagePressed;
	private Drawable rightViewImagePressed;

	public ShelfSearchButtonLayout(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		init(context, attrs);
	}

	public ShelfSearchButtonLayout(Context context, AttributeSet attrs) {
		super(context, attrs);
		init(context, attrs);
	}

	public ShelfSearchButtonLayout(Context context) {
		super(context);
		init(context, null);
	}

	private void init(Context context, AttributeSet attrs) {
		LayoutInflater layoutInflater = (LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		layoutInflater.inflate(R.layout.shelf_search_button_layout, this);

		leftView = (ImageView) findViewById(R.id.leftView);
		middleView = (ImageView) findViewById(R.id.middleView);
		rightView = (ImageView) findViewById(R.id.rightView);

		if (attrs != null) {
			TypedArray typedArray = context.obtainStyledAttributes(attrs, R.styleable.ShelfSearchButtonLayout);

			leftViewImageId = typedArray.getResourceId(R.styleable.ShelfSearchButtonLayout_leftView, R.drawable.shelf_search_left);
			leftView.setBackgroundResource(leftViewImageId);

			middleViewImageId = typedArray.getResourceId(R.styleable.ShelfSearchButtonLayout_middleView, R.drawable.shelf_search_middle);
			middleView.setBackgroundResource(middleViewImageId);

			rightViewImageId = typedArray.getResourceId(R.styleable.ShelfSearchButtonLayout_rightView, R.drawable.shelf_search_right);
			rightView.setBackgroundResource(rightViewImageId);

			leftViewImagePressedId = typedArray.getResourceId(R.styleable.ShelfSearchButtonLayout_leftViewPressed,
					R.drawable.shelf_search_pressed_left);
			middleViewImagePressedId = typedArray.getResourceId(R.styleable.ShelfSearchButtonLayout_middleViewPressed,
					R.drawable.shelf_search_pressed_middle);
			rightViewImagePressedId = typedArray.getResourceId(R.styleable.ShelfSearchButtonLayout_rightViewPressed,
					R.drawable.shelf_search_pressed_right);

			typedArray.recycle();
		}
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		switch (event.getAction()) {
		case MotionEvent.ACTION_DOWN:
			setPressedImages();

			break;
		case MotionEvent.ACTION_CANCEL:
			setDefaultImages();

			break;
		case MotionEvent.ACTION_UP:
			setDefaultImages();

			break;
		default:
			break;
		}

		return super.onTouchEvent(event);
	}

	public void setDefaultImages() {
		if (leftViewImage != null) {
			leftView.setBackgroundDrawable(leftViewImage);
			middleView.setBackgroundDrawable(middleViewImage);
			rightView.setBackgroundDrawable(rightViewImage);
		} else {
			leftView.setBackgroundResource(leftViewImageId);
			middleView.setBackgroundResource(middleViewImageId);
			rightView.setBackgroundResource(rightViewImageId);
		}
	}

	private void setPressedImages() {
		if (leftViewImage != null) {
			leftView.setBackgroundDrawable(leftViewImagePressed);
			middleView.setBackgroundDrawable(middleViewImagePressed);
			rightView.setBackgroundDrawable(rightViewImagePressed);
		} else {
			leftView.setBackgroundResource(leftViewImagePressedId);
			middleView.setBackgroundResource(middleViewImagePressedId);
			rightView.setBackgroundResource(rightViewImagePressedId);
		}
	}

	public void setLeftViewImage(Drawable leftViewImage) {
		this.leftViewImage = leftViewImage;
		leftView.setBackgroundDrawable(leftViewImage);
	}

	public void setMiddleViewImage(Drawable middleViewImage) {
		this.middleViewImage = middleViewImage;
		middleView.setBackgroundDrawable(middleViewImage);
	}

	public void setRightViewImage(Drawable rightViewImage) {
		this.rightViewImage = rightViewImage;
		rightView.setBackgroundDrawable(rightViewImage);
	}

	public void setLeftViewImagePressed(Drawable leftViewImagePressed) {
		this.leftViewImagePressed = leftViewImagePressed;
	}

	public void setMiddleViewImagePressed(Drawable middleViewImagePressed) {
		this.middleViewImagePressed = middleViewImagePressed;
	}

	public void setRightViewImagePressed(Drawable rightViewImagePressed) {
		this.rightViewImagePressed = rightViewImagePressed;
	}
}

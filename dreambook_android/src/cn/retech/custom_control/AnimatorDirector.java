package cn.retech.custom_control;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import android.animation.Animator;
import android.animation.Animator.AnimatorListener;
import android.animation.AnimatorSet;
import android.animation.ObjectAnimator;
import android.view.View;

public class AnimatorDirector {
	public static class AnimatorBuilder {
		// key:需要执行动画的属性名 value:属性变更的值
		private final Map<String, Object> propertyAndValues = new HashMap<String, Object>();
		private Long duration = 300l;// 动画执行时间
		private AnimatorListener animatorListener;// 动画监听
		private View custView;// 执行动画的View

		public AnimatorBuilder view(View view) {
			this.custView = view;

			return this;
		}

		public AnimatorBuilder alpha(float alpha) {
			this.propertyAndValues.put("alpha", alpha);

			return this;
		}

		public AnimatorBuilder x(float x) {
			this.propertyAndValues.put("X", x);

			return this;
		}

		public AnimatorBuilder y(float y) {
			this.propertyAndValues.put("Y", y);

			return this;
		}

		public AnimatorBuilder translationX(float x) {
			this.propertyAndValues.put("translationX", x);

			return this;
		}

		public AnimatorBuilder translationY(float y) {
			this.propertyAndValues.put("translationY", y);

			return this;
		}

		public AnimatorBuilder scaleX(float x) {
			this.propertyAndValues.put("scaleX", x);

			return this;
		}

		public AnimatorBuilder scaleY(float y) {
			this.propertyAndValues.put("scaleY", y);

			return this;
		}

		public AnimatorBuilder setDuration(long duration) {
			this.duration = duration;

			return this;
		}

		public AnimatorBuilder setListener(AnimatorListener animationListener) {
			this.animatorListener = animationListener;

			return this;
		}
	}

	public static AnimatorSet animate(AnimatorBuilder builder) {
		if (builder.custView == null || builder.propertyAndValues.size() == 0) {
			return null;
		}

		// 通过迭代Map的键值对来生成相应的动画对象
		List<Animator> animators = new ArrayList<Animator>();
		for (String property : builder.propertyAndValues.keySet()) {
			Object propertyValue = builder.propertyAndValues.get(property);

			ObjectAnimator objectAnimator = null;
			if (propertyValue instanceof Float) {
				objectAnimator = ObjectAnimator.ofFloat(builder.custView, property, (Float) propertyValue);
			} else if (propertyValue instanceof Integer) {
				objectAnimator = ObjectAnimator.ofInt(builder.custView, property, (Integer) propertyValue);
			}

			if (null != objectAnimator) {
				objectAnimator.setDuration(builder.duration);
				animators.add(objectAnimator);
			}
		}

		AnimatorSet animatorSet = new AnimatorSet();

		if (builder.animatorListener != null) {
			animatorSet.addListener(builder.animatorListener);
		}

		animatorSet.playTogether(animators);// 设置所有动画同时播放
		animatorSet.start();

		return animatorSet;
	}
}

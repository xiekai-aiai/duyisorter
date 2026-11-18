# uncompyle6 version 3.9.3
# Python bytecode version base 3.6 (3379)
# Decompiled from: Python 3.8.20 (default, Oct  3 2024, 10:25:41) 
# [Clang 14.0.6 ]
# Embedded file name: norm_train.py
import numpy as np
from sklearn.svm import SVC
from sklearn.model_selection import train_test_split
from sklearn.metrics import classification_report, accuracy_score
from sklearn.preprocessing import StandardScaler
import sys, os
try:
    import importlib.resources as resources
except ImportError:
    import importlib_resources as resources

class ColorSorterSVM:

    def __init__(self, bg_threshold1=70, bg_threshold2=40, min_contour_area=100, color_mode='rgb', feat_dim=9, scalar_mode='mean'):
        """
        初始化ColorSorterSVM类。
        Args:
            bg_threshold (tuple): 背景基准像素值。
            deviation (int): 背景颜色偏差范围。
            min_contour_area (int): 最小轮廓面积，用于过滤小的前景区域。
        """
        self.bg_threshold1 = bg_threshold1
        self.bg_threshold2 = bg_threshold2
        self.min_contour_area = min_contour_area
        self.color_mode = color_mode
        self.feat_dim = feat_dim
        self.svm = None
        self.w = None
        self.b = None
        self.scalar_mode = scalar_mode
        self.scalar = None

    def load_range_param(self, param_file):
        normalization_data = {}
        with open(param_file, "r") as file:
            lines = file.readlines()
            for line in lines[2:]:
                parts = line.strip().split()
                feature_index = int(parts[0]) - 1
                min_value = float(parts[1])
                max_value = float(parts[2])
                normalization_data[feature_index] = (min_value, max_value)

        self.normalization_data = normalization_data

    def load_model_data(self, data_file):
        data = []
        labels = []
        with open(data_file, "r") as file:
            for line in file:
                parts = line.strip().split()
                labels.append(int(parts[0]))
                features = [float(part.split(":")[1]) for part in parts[1:]]
                data.append(features)

        features = np.array(data)
        self.feat_dim = features.shape[1]
        self.scalar = None
        return (features, np.array(labels))

    def preprocess(self, data):
        if data.shape[1] != self.feat_dim:
            data = data[:, :self.feat_dim]
        else:
            if self.scalar_mode == "mean":
                return self.preprocess_scalar(data)
            if self.scalar_mode == "min_max":
                return self.preprocess_min_max(data, self.normalization_data)

    def preprocess_scalar(self, data):
        if self.scalar is None:
            scalar = StandardScaler()
            data = scalar.fit_transform(data)
            self.scalar = scalar
        else:
            data = self.scalar.transform(data)
        return data

    def preprocess_min_max(self, data, range_param):
        for i in range(data.shape[1]):
            middle_value = (range_param[i][1] + range_param[i][0]) / 2
            data[:, i] = 2 * (data[:, i] - middle_value) / (range_param[i][1] - range_param[i][0])

        return data

    def extract_pixel_features(self, img, label):
        mask = self.get_fg_mask(img)
        fg_pixels = img[mask > 0]
        b, g, r = fg_pixels[:, 0], fg_pixels[:, 1], fg_pixels[:, 2]
        if self.color_mode == "rgb":
            features = np.column_stack((r, g, b, r * g, r * b, g * b, r ** 2, g ** 2, b ** 2))
        else:
            features = np.column_stack((b, g, r, b * g, b * r, g * r, b ** 2, g ** 2, r ** 2))
        labels = np.full((features.shape[0],), label)
        return (features, labels)

    def train_svm_classifier(self, X, y, test_ratio=0.2):
        print("Training SVM classifier...")
        X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=test_ratio, random_state=42)
        self.svm = SVC(kernel="linear", C=5.0, random_state=42)
        self.svm.fit(X_train, y_train)
        self.w = self.svm.coef_[0].copy()
        self.b = self.svm.intercept_[0].copy()
        y_pred = self.svm.predict(X_test)
        print("Classification Report:")
        print(classification_report(y_test, y_pred))
        print("Accuracy:", accuracy_score(y_test, y_pred))

    def classify_with_hyperplane(self, features):
        decision_values = np.dot(features, self.w) + self.b
        return (decision_values > 0).astype(np.uint8)

    def classify_with_merged_param(self, features):
        decision_values = np.dot(features, self.w) + self.b
        return (decision_values > 0).astype(np.uint8)

    def merge_param(self):
        print("before merge param:")
        print("w:", self.w)
        print("b:", self.b)
        for i in range(self.feat_dim):
            self.b -= self.w[i] * self.scalar.mean_[i] / self.scalar.scale_[i]
            self.w[i] = self.w[i] / self.scalar.scale_[i]

        self.b = self.b * 1000000
        self.w = self.w * 1000000
        print("after merge param:")
        print("w:", self.w)
        print("b:", self.b)

    def random_downsample(self, X, y, sample_size):
        """
        对数据集进行随机降采样
        Args:
            X: 特征矩阵 (N, d)
            y: 标签数组 (N,)
            sample_size: 降采样后的样本数量

        Returns:
            X_sampled, y_sampled: 降采样后的特征和标签
        """
        if sample_size >= len(y):
            return (X, y)
        else:
            indices = np.random.choice((len(y)), size=sample_size, replace=False)
            return (X[indices], y[indices])


if __name__ == "__main__":
    if not len(sys.argv) == 2:
        raise AssertionError("输入参数数量必须为1")
    else:
        data_file = sys.argv[1]
        assert os.path.exists(data_file), "输入{}不存在".format(data_file)
    svm_train = ColorSorterSVM(color_mode="rgb", scalar_mode="mean", feat_dim=6)
    data, labels = svm_train.load_model_data(data_file)
    norm_data = svm_train.preprocess(data)
    svm_train.train_svm_classifier(norm_data, labels)
    svm_train.merge_param()
    save_file = data_file + ".model"
    print("系数:", svm_train.w)
    print("偏置:", svm_train.b)
    with open(save_file, "w") as f:
        for d in svm_train.w:
            f.write("{}\n".format(d))

        f.write("{}".format(svm_train.b))

# okay decompiling norm_train.pyc

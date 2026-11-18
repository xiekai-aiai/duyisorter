// train.cpp - C++ 版 SVM 训练程序（对应 norm_train.py）
// 编译: g++ -O2 -o svm_train train.cpp svm.cpp
// 用法: ./svm_train <train.txt> [max_samples]
//       不带 max_samples 时使用全部样本；带上且样本数超过该值时随机降采样到该数量
//       输出 <train.txt>.model

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include "svm.h"

using namespace std;

// ---------- 数据读取（libsvm 格式: label idx:val idx:val ...） ----------
struct Dataset {
    int n = 0;
    int d = 0;
    vector<vector<double>> X;
    vector<double> y;
};

static bool load_data(const string& path, Dataset& ds) {
    FILE* f = fopen(path.c_str(), "r");
    if (!f) {
        fprintf(stderr, "无法打开文件: %s\n", path.c_str());
        return false;
    }
    char line[8192];
    int cnt = 0;
    while (fgets(line, sizeof(line), f)) {
        char* p = line;
        char* end = nullptr;
        double label = strtod(p, &end);  // 标签
        p = end;
        ds.y.push_back(label);

        vector<double> feat;
        while (true) {
            while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
            if (*p == '\0') break;
            strtol(p, &end, 10);  // 索引 idx（跳过）
            if (end == p) break;
            p = end;
            if (*p == ':') p++;
            double val = strtod(p, &end);  // 特征值
            if (end == p) break;
            p = end;
            feat.push_back(val);
        }
        ds.X.push_back(feat);
        if (++cnt % 2000 == 0) {
            printf("  读取中... %d 行\n", cnt);
            fflush(stdout);
        }
    }
    fclose(f);
    printf("  读取完成，共 %d 行\n", cnt);
    ds.n = (int)ds.X.size();
    ds.d = ds.X.empty() ? 0 : (int)ds.X[0].size();
    return ds.n > 0;
}

// ---------- StandardScaler（对应 sklearn.preprocessing.StandardScaler） ----------
struct StandardScaler {
    vector<double> mean;
    vector<double> scale;

    void fit(const Dataset& ds) {
        int n = ds.n, d = ds.d;
        mean.assign(d, 0.0);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < d; j++)
                mean[j] += ds.X[i][j];
        for (int j = 0; j < d; j++)
            mean[j] /= n;

        scale.assign(d, 0.0);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < d; j++) {
                double diff = ds.X[i][j] - mean[j];
                scale[j] += diff * diff;
            }
        for (int j = 0; j < d; j++) {
            scale[j] = sqrt(scale[j] / n);  // 总体标准差 ddof=0
            if (scale[j] == 0.0) scale[j] = 1.0;
        }
    }

    double transform(double x, int j) const {
        return (x - mean[j]) / scale[j];
    }
};

// ---------- 随机划分（标准库实现，类似 train_test_split test_size=0.2） ----------
static vector<int> split_indices(int n, int& n_train, int& n_test) {
    n_test = (int)ceil(0.2 * n);
    n_train = n - n_test;
    vector<int> idx(n);
    for (int i = 0; i < n; i++) idx[i] = i;
    mt19937 rng(42);
    shuffle(idx.begin(), idx.end(), rng);
    return idx;
}

// ---------- 随机降采样（样本数超过上限时，随机抽取 max_samples 个） ----------
static void downsample_dataset(Dataset& ds, int max_samples) {
    if (max_samples <= 0 || ds.n <= max_samples) return;

    vector<int> idx(ds.n);
    for (int i = 0; i < ds.n; i++) idx[i] = i;
    mt19937 rng(42);
    shuffle(idx.begin(), idx.end(), rng);

    vector<vector<double>> X;
    vector<double> y;
    X.reserve(max_samples);
    y.reserve(max_samples);
    for (int i = 0; i < max_samples; i++) {
        X.push_back(ds.X[idx[i]]);
        y.push_back(ds.y[idx[i]]);
    }
    ds.X.swap(X);
    ds.y.swap(y);
    ds.n = max_samples;
}

int main(int argc, char** argv) {
    if (argc != 2 && argc != 3) {
        fprintf(stderr, "用法: %s <train.txt> [max_samples]\n", argv[0]);
        fprintf(stderr, "  max_samples: 可选，样本数超过该值时随机降采样到该数量\n");
        return 1;
    }
    string data_file = argv[1];

    int max_samples = 0;    // 0 表示不限制
    if (argc == 3) {
        char* end = nullptr;
        long v = strtol(argv[2], &end, 10);
        if (end == argv[2] || *end != '\0' || v <= 0) {
            fprintf(stderr, "无效的采样数值: %s\n", argv[2]);
            return 1;
        }
        max_samples = (int)v;
    }

    string save_file = data_file + ".model";

    Dataset ds;
    if (!load_data(data_file, ds)) {
        fprintf(stderr, "读取训练数据失败: %s\n", data_file.c_str());
        return 1;
    }
    int d = ds.d;
    printf("样本数: %d, 特征数: %d\n", ds.n, d);

    // 样本数超过设定值时降采样
    if (max_samples > 0 && ds.n > max_samples) {
        printf("样本数超过 %d，降采样 ...\n", max_samples);
        downsample_dataset(ds, max_samples);
        printf("降采样后样本数: %d\n", ds.n);
    }

    printf("标准化特征 ...\n");
    // 1. 标准化（全量数据 fit）
    StandardScaler scaler;
    scaler.fit(ds);
    printf("标准化完成\n");

    // 2. 随机划分
    int n_train, n_test;
    vector<int> idx = split_indices(ds.n, n_train, n_test);
    printf("训练集: %d, 测试集: %d\n", n_train, n_test);

    // 3. 构造 libsvm problem（训练集，标准化后的特征）
    svm_problem prob;
    prob.l = n_train;
    prob.y = new double[n_train];
    prob.x = new svm_node*[n_train];
    for (int i = 0; i < n_train; i++) {
        int orig = idx[i];
        prob.y[i] = ds.y[orig];
        prob.x[i] = new svm_node[d + 1];
        for (int j = 0; j < d; j++) {
            prob.x[i][j].index = j + 1;
            prob.x[i][j].value = scaler.transform(ds.X[orig][j], j);
        }
        prob.x[i][d].index = -1;
        prob.x[i][d].value = 0.0;
    }

    // 4. libsvm 参数（对应 SVC(kernel="linear", C=5.0, tol=1e-3)）
    svm_parameter param;
    param.svm_type = C_SVC;
    param.kernel_type = LINEAR;
    param.degree = 3;
    param.gamma = 0.0;   // linear 核不使用 gamma
    param.coef0 = 0.0;
    param.cache_size = 40.0;   // 板端内存小，调低缓存（仅影响训练速度，不影响结果）
    param.eps = 1e-3;
    param.C = 5.0;
    param.nr_weight = 0;
    param.weight_label = nullptr;
    param.weight = nullptr;
    param.nu = 0.5;
    param.p = 0.1;
    param.shrinking = 1;
    param.probability = 0;

    const char* err = svm_check_parameter(&prob, &param);
    if (err) {
        fprintf(stderr, "参数错误: %s\n", err);
        return 1;
    }

    printf("训练 SVM（线性核, C=5.0, 训练集 %d 样本）...\n", n_train);
    fflush(stdout);
    svm_model* model = svm_train(&prob, &param);
    printf("训练完成，支持向量数: %d\n", model->l);

    // 5. 从 model 提取线性 w/b：w = sum(sv_coef * SV)，b = -rho
    vector<double> w(d, 0.0);
    int total_sv = model->l;
    for (int i = 0; i < total_sv; i++) {
        double coef = model->sv_coef[0][i];
        for (const svm_node* p = model->SV[i]; p->index != -1; p++) {
            int j = p->index - 1;
            if (j >= 0 && j < d) w[j] += coef * p->value;
        }
    }
    double b = -model->rho[0];

    // 6. merge_param：把 scaler 折入 w/b，再乘 1e6
    for (int j = 0; j < d; j++) {
        b -= w[j] * scaler.mean[j] / scaler.scale[j];
        w[j] = w[j] / scaler.scale[j];
    }
    b *= 1000000.0;
    for (int j = 0; j < d; j++) w[j] *= 1000000.0;

    // 7. 写 .model：w 每行 + b 最后一行
    FILE* fout = fopen(save_file.c_str(), "w");
    if (!fout) {
        fprintf(stderr, "无法写入: %s\n", save_file.c_str());
        return 1;
    }
    for (int j = 0; j < d; j++)
        fprintf(fout, "%.17g\n", w[j]);
    fprintf(fout, "%.17g", b);
    fclose(fout);

    printf("系数 w:");
    for (int j = 0; j < d; j++) printf(" %.10g", w[j]);
    printf("\n偏置 b: %.10g\n", b);
    printf("已保存: %s\n", save_file.c_str());

    // 8. 在测试集上计算准确率
    int n_correct = 0;
    int cm[4] = {0, 0, 0, 0};  // 真-1预-1, 真-1预1, 真1预-1, 真1预1
    svm_node* test_x = new svm_node[d + 1];
    for (int k = n_train; k < ds.n; k++) {
        int orig = idx[k];
        for (int j = 0; j < d; j++) {
            test_x[j].index = j + 1;
            test_x[j].value = scaler.transform(ds.X[orig][j], j);
        }
        test_x[d].index = -1;
        test_x[d].value = 0.0;
        int pred = (int)svm_predict(model, test_x);
        int real = (int)ds.y[orig];
        if (pred == real) n_correct++;
        cm[(real < 0 ? 0 : 2) + (pred < 0 ? 0 : 1)]++;
    }
    delete[] test_x;
    printf("准确率 Accuracy: %.2f%% (%d/%d)\n",
           100.0 * n_correct / n_test, n_correct, n_test);
    printf("混淆矩阵 (行=真实 列=预测): [[%d %d] [%d %d]]\n",
           cm[0], cm[1], cm[2], cm[3]);

    // 清理
    svm_free_and_destroy_model(&model);
    for (int i = 0; i < n_train; i++) delete[] prob.x[i];
    delete[] prob.x;
    delete[] prob.y;

    return 0;
}

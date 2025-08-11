plugins {
    alias(libs.plugins.android.application)
    alias(libs.plugins.kotlin.android)
}

// ---- Version from gradle.properties ----
val major = providers.gradleProperty("VERSION_MAJOR").get().toInt()
val minor = providers.gradleProperty("VERSION_MINOR").get().toInt()
val patch = providers.gradleProperty("VERSION_PATCH").get().toInt()
val beta = providers.gradleProperty("VERSION_BETA").get().toInt()
val postfix = providers.gradleProperty("VERSION_POSTFIX").get()

fun makeVersionCode(maj: Int, min: Int, pat: Int, beta: Int): Int =
    if (beta > 0) maj * 1_000_000 + min * 10_000 + (pat - 1) * 100 + beta
    else maj * 1_000_000 + min * 10_000 + pat * 100

fun makeVersionName(maj: Int, min: Int, pat: Int, beta: Int, postfix: String): String =
    if (beta > 0) "$maj.$min.$pat $postfix $beta" else "$maj.$min.$pat"

android {
    namespace = "com.mediatek.vkdemo"
    compileSdk = 36

    defaultConfig {
        applicationId = "com.mediatek.vkdemo"
        minSdk = 31
        targetSdk = 36
        versionCode = 1
        versionName = "1.0"
        versionCode = makeVersionCode(major, minor, patch, beta)
        versionName = makeVersionName(major, minor, patch, beta, postfix)
        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_11
        targetCompatibility = JavaVersion.VERSION_11
    }
    kotlinOptions {
        jvmTarget = "11"
    }
    buildFeatures {
        viewBinding = true
        buildConfig = true
        prefab = true
    }
    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }
}

dependencies {
    implementation(libs.androidx.core.ktx)
    implementation(libs.androidx.appcompat)
    implementation(libs.material)
    implementation(libs.androidx.games.activity)
    testImplementation(libs.junit)
    androidTestImplementation(libs.androidx.junit)
    androidTestImplementation(libs.androidx.espresso.core)
}
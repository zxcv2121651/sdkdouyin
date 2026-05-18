plugins {
    id("com.android.application")
    id("org.jetbrains.kotlin.android")
    id("dagger.hilt.android.plugin")
}

android {
    namespace = "com.video.demo"
    compileSdk = 34
    defaultConfig {
        applicationId = "com.video.demo"
        minSdk = 24
        targetSdk = 34
        versionCode = 1
        versionName = "1.0"
    }
}

dependencies {
    // App 是空壳，聚合所有 feature
    implementation(project(":feature:editor"))
    implementation(project(":feature:export"))
    implementation(project(":feature:camera"))
    implementation(project(":core:common"))

    // Hilt DI
    implementation("com.google.dagger:hilt-android:2.48")
    // kapt("com.google.dagger:hilt-android-compiler:2.48")
}

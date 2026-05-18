pluginManagement {
    repositories {
        google()
        mavenCentral()
        gradlePluginPortal()
    }
}
dependencyResolutionManagement {
    repositoriesMode.set(RepositoriesMode.FAIL_ON_PROJECT_REPOS)
    repositories {
        google()
        mavenCentral()
    }
}

rootProject.name = "VideoSDKDemo"

include(":app")
include(":core:common")
include(":core:designsystem")
include(":feature:editor")
include(":feature:export")
include(":feature:camera")
// 也可以直接 include SDK
// include(":sdk")
// project(":sdk").projectDir = new File("../../")

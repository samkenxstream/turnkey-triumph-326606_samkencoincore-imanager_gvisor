load("//tools:defs.bzl", "go_library")

package(
    default_applicable_licenses = ["//:license"],
    licenses = ["notice"],
)

go_library(
    name = "cli",
    srcs = ["main.go"],
    visibility = [
        "//:__pkg__",
        "//runsc:__pkg__",
    ],
    deps = [
        "//pkg/coverage",
        "//pkg/log",
        "//pkg/refs",
        "//pkg/sentry/platform",
        "//pkg/sentry/syscalls/linux",
        "//runsc/cmd",
        "//runsc/cmd/trace",
        "//runsc/cmd/util",
        "//runsc/config",
        "//runsc/flag",
        "//runsc/specutils",
        "//runsc/version",
        "@com_github_google_subcommands//:go_default_library",
        "@org_golang_x_sys//unix:go_default_library",
    ],
)

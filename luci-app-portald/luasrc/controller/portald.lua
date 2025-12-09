module("luci.controller.portald", package.seeall)

function index()
    if not nixio.fs.access("/usr/sbin/portald") then
        return
    end

    entry({"admin", "network", "portald"},
          firstchild(), _("Portal 认证"), 90).dependent = true

    entry({"admin", "network", "portald", "config"},
          cbi("portald/main"), _("配置"), 10).dependent = true

    entry({"admin", "network", "portald", "status"},
          template("portald/index"), _("在线管理"), 20).dependent = true
end
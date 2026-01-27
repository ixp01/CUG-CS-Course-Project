import { useState, useMemo } from 'react';
import { Search, Eye, Trash2, Calendar, Package, BarChart3 } from 'lucide-react';

interface Detection {
  id: string;
  deviceInfo: any;
  experimentData: any;
  resultData: any;
  timestamp: number;
}

interface DataQueryProps {
  detections: Detection[];
  onView: (detection: Detection) => void;
}

export function DataQuery({ detections, onView }: DataQueryProps) {
  const [searchTerm, setSearchTerm] = useState('');
  const [dateFilter, setDateFilter] = useState('');
  const [showStats, setShowStats] = useState(false);

  const filteredDetections = useMemo(() => {
    return detections.filter(detection => {
      const matchesSearch = !searchTerm || 
        detection.deviceInfo?.productCode?.toLowerCase().includes(searchTerm.toLowerCase()) ||
        detection.deviceInfo?.productName?.toLowerCase().includes(searchTerm.toLowerCase()) ||
        detection.deviceInfo?.manufacturer?.toLowerCase().includes(searchTerm.toLowerCase());
      
      const matchesDate = !dateFilter ||
        new Date(detection.timestamp).toISOString().split('T')[0] === dateFilter;
      
      return matchesSearch && matchesDate;
    });
  }, [detections, searchTerm, dateFilter]);

  const stats = useMemo(() => {
    return {
      total: detections.length,
      thisMonth: detections.filter(d => {
        const date = new Date(d.timestamp);
        const now = new Date();
        return date.getMonth() === now.getMonth() && date.getFullYear() === now.getFullYear();
      }).length,
      manufacturers: new Set(detections.map(d => d.deviceInfo?.manufacturer).filter(Boolean)).size,
      avgPerDay: detections.length > 0 ? (detections.length / 30).toFixed(1) : '0'
    };
  }, [detections]);

  const handleDelete = (id: string) => {
    if (confirm('确定要删除这条检测记录吗？')) {
      const updated = detections.filter(d => d.id !== id);
      localStorage.setItem('detections', JSON.stringify(updated));
      window.location.reload();
    }
  };

  return (
    <div className="space-y-6">
      {/* Statistics Cards */}
      <div className="grid grid-cols-1 md:grid-cols-4 gap-4">
        <div className="bg-gradient-to-br from-blue-500 to-blue-600 text-white rounded-lg shadow-md p-6">
          <div className="flex items-center justify-between">
            <div>
              <p className="text-blue-100">总检测数</p>
              <p className="mt-2">{stats.total}</p>
            </div>
            <Package className="w-12 h-12 text-blue-200" />
          </div>
        </div>

        <div className="bg-gradient-to-br from-green-500 to-green-600 text-white rounded-lg shadow-md p-6">
          <div className="flex items-center justify-between">
            <div>
              <p className="text-green-100">本月检测</p>
              <p className="mt-2">{stats.thisMonth}</p>
            </div>
            <Calendar className="w-12 h-12 text-green-200" />
          </div>
        </div>

        <div className="bg-gradient-to-br from-purple-500 to-purple-600 text-white rounded-lg shadow-md p-6">
          <div className="flex items-center justify-between">
            <div>
              <p className="text-purple-100">制造商数</p>
              <p className="mt-2">{stats.manufacturers}</p>
            </div>
            <BarChart3 className="w-12 h-12 text-purple-200" />
          </div>
        </div>

        <div className="bg-gradient-to-br from-orange-500 to-orange-600 text-white rounded-lg shadow-md p-6">
          <div className="flex items-center justify-between">
            <div>
              <p className="text-orange-100">日均检测</p>
              <p className="mt-2">{stats.avgPerDay}</p>
            </div>
            <BarChart3 className="w-12 h-12 text-orange-200" />
          </div>
        </div>
      </div>

      {/* Search and Filter */}
      <div className="bg-white rounded-lg shadow-md p-6">
        <h2 className="text-slate-900 mb-4">检测数据查询</h2>
        
        <div className="flex gap-4 mb-6">
          <div className="flex-1 relative">
            <Search className="absolute left-3 top-1/2 transform -translate-y-1/2 w-5 h-5 text-slate-400" />
            <input
              type="text"
              placeholder="搜索产品编号、名称或制造商..."
              value={searchTerm}
              onChange={(e) => setSearchTerm(e.target.value)}
              className="w-full pl-10 pr-4 py-3 border border-slate-300 rounded-lg focus:ring-2 focus:ring-blue-500 focus:border-blue-500 outline-none"
            />
          </div>
          <div className="w-64">
            <input
              type="date"
              value={dateFilter}
              onChange={(e) => setDateFilter(e.target.value)}
              className="w-full px-4 py-3 border border-slate-300 rounded-lg focus:ring-2 focus:ring-blue-500 focus:border-blue-500 outline-none"
            />
          </div>
          <button
            onClick={() => {
              setSearchTerm('');
              setDateFilter('');
            }}
            className="px-6 py-3 text-slate-700 border-2 border-slate-300 rounded-lg hover:bg-slate-50 transition"
          >
            重置
          </button>
        </div>

        {/* Results */}
        {filteredDetections.length === 0 ? (
          <div className="text-center py-12">
            <Package className="w-16 h-16 text-slate-300 mx-auto mb-4" />
            <p className="text-slate-500">暂无检测记录</p>
          </div>
        ) : (
          <div className="overflow-x-auto">
            <table className="w-full">
              <thead>
                <tr className="border-b-2 border-slate-200">
                  <th className="text-left px-4 py-3 text-slate-700">产品编号</th>
                  <th className="text-left px-4 py-3 text-slate-700">产品名称</th>
                  <th className="text-left px-4 py-3 text-slate-700">制造商</th>
                  <th className="text-left px-4 py-3 text-slate-700">送检日期</th>
                  <th className="text-left px-4 py-3 text-slate-700">检测日期</th>
                  <th className="text-right px-4 py-3 text-slate-700">操作</th>
                </tr>
              </thead>
              <tbody>
                {filteredDetections.map((detection) => (
                  <tr key={detection.id} className="border-b border-slate-200 hover:bg-slate-50">
                    <td className="px-4 py-3">
                      {detection.deviceInfo?.productCode || '-'}
                    </td>
                    <td className="px-4 py-3">
                      {detection.deviceInfo?.productName || '-'}
                    </td>
                    <td className="px-4 py-3">
                      {detection.deviceInfo?.manufacturer || '-'}
                    </td>
                    <td className="px-4 py-3">
                      {detection.deviceInfo?.inspectionDate || '-'}
                    </td>
                    <td className="px-4 py-3">
                      {new Date(detection.timestamp).toLocaleDateString('zh-CN')}
                    </td>
                    <td className="px-4 py-3">
                      <div className="flex justify-end gap-2">
                        <button
                          onClick={() => onView(detection)}
                          className="p-2 text-blue-600 hover:bg-blue-50 rounded transition"
                          title="查看报告"
                        >
                          <Eye className="w-5 h-5" />
                        </button>
                        <button
                          onClick={() => handleDelete(detection.id)}
                          className="p-2 text-red-600 hover:bg-red-50 rounded transition"
                          title="删除记录"
                        >
                          <Trash2 className="w-5 h-5" />
                        </button>
                      </div>
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
        )}

        <div className="mt-4 text-slate-600">
          共找到 <span>{filteredDetections.length}</span> 条记录
        </div>
      </div>

      {/* Manufacturer Statistics */}
      {detections.length > 0 && (
        <div className="bg-white rounded-lg shadow-md p-6">
          <h2 className="text-slate-900 mb-4">制造商统计</h2>
          <div className="space-y-3">
            {Array.from(new Set(detections.map(d => d.deviceInfo?.manufacturer).filter(Boolean))).map(manufacturer => {
              const count = detections.filter(d => d.deviceInfo?.manufacturer === manufacturer).length;
              const percentage = ((count / detections.length) * 100).toFixed(1);
              return (
                <div key={manufacturer as string} className="flex items-center gap-4">
                  <div className="w-40 text-slate-700">{manufacturer}</div>
                  <div className="flex-1 bg-slate-200 rounded-full h-8 overflow-hidden">
                    <div 
                      className="bg-blue-600 h-full flex items-center justify-end px-3 text-white transition-all duration-500"
                      style={{ width: `${percentage}%` }}
                    >
                      {percentage}%
                    </div>
                  </div>
                  <div className="w-16 text-right text-slate-600">{count} 台</div>
                </div>
              );
            })}
          </div>
        </div>
      )}
    </div>
  );
}

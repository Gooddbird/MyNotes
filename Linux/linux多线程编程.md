# 一、对象的线程安全

常见的竞争条件

- 析构一个对象时，如何判断是否正在有其他线程执行该对象的成员函数。
- 调用一个对象的成员时，如何得知该对象是否存活，是否正在被其他线程析构。

对象构造线程安全

- **不要在构造期间暴露this指针**， 因为在构造期间对象还没完成初始化，泄露this指针给其他对象的话，别的线程有可能访问这个半成品。
1. 不要在构造函数中注册任何回调。
2. 不要在构造函数中把this传递给跨线程的对象。
3. 即使在构造函数最后一行也不行。因为可能是个基类，其实执行到基类构造函数最后一行还会去执行子类的构造函数，此时对象还是半成品。


### 对象销毁线程安全
难点在于对象析构时会析构其mutex成员，此时就不能用mutex成员来保护临界区。

```
class Foo {
public:
    Foo() {};
    ~Foo() {
        MutexLockGuard lock(m_mutex);
        (1)// free....
    }


    update() {
        MutexLockGuard lock(m_mutex); (2)
        // .....
    }


private:
    MutexLock m_mutex;


};
```
当线程 A B同时看到Foo的一个对象x， 线程A准备销毁x，线程B准备调用x->update
```
// thread A
delete x;
x = NULL;


// thread B
if(x) {
    x->update();
}

```
尽管A销毁对象后把x置为NULL，尽管B在调用前先判断了x是否为NULL。但可能： 
1. A执行到析构函数的(1)处取得了锁，继续往下执行。
2. 此时A还正在执行析构函数，x的值还不为NULL，则B能通过if(x)检测然后到达update函数(2)处，此时由于A拿到了锁，B会阻塞在(2)处。
3. 然后A执行析构函数会把m_mutex销毁掉，那么此时(2)处的情况会怎么就不得而知了。。
   
这个例子说明作为成员变量的mutex是不能有效保护对象的析构，事实上对象的析构过程也不应该需要保护，只有当别的线程都访问不到这个对象的时候析构才是安全的，也就是之前的竞争条件。

### 线程安全的Observer模式

```
class Observer {
public:
    virtual ~Observer();
    virtual void update() = 0;
    // ...
};

class Observable {
public:
    void register(Observer* x);
    void unregister(Observer* x);

    void notifyObservers() {
        for(Observer* x : obs) {
            x->update();
        }
    }
private:
    std::vector<Observer*> obs;
}
```
当Observable执行notify。。时，如何得知Observer 对象x还活着？

方法1：在Observer析构时调用unregister来解注册，删除obs里面的这个Observer对象.
```
class Observer {
// 同上
    Observer::~Observer() {
        t->unregister(this);// (1)
    }

private:
    Observable* t;
}
```
问题：
1. 执行（1）处代码时如何得知对象t还活着？
2. 就算t是个一直存在的对象，当线程A执行到(1)处之前还没来得及unregister，线程B就执行到了notify方法中x->update()这一行。也就是说，这个对象x此时正在析构，那么调用x的任何**非静态成员函数**都不安全，更何况还是虚函数。除此之外，Observer是个基类，若执行update时候派生类对象已经析构了，那么此时结果将会不可预测。

这个例子说明Observable不应该保存Observer* ，而是别的什么东西可以指明该对象是否存活的东西。也就是智能指针。

智能指针

1. shared_ptr: 强引用（即用铁丝绑住对象），只要有一个shared_ptr指向该对象，该对象就不可能被销毁。当最后一个shared_ptr析构或reset时，该对象立马被销毁。
2. weak_ptr: 弱引用(用棉线绑住对象)。但可以得知对象是否存活，若存活可将weak_ptr提升为shared_ptr，若对象已死则提升失败返回一个空的shared_ptr。且提升行为lock()是线程安全的。  

    利用智能指针，Observable保存一个 vector<weak_ptr<Observer>> obs,每次从obs取一个weak_ptr尝试提升，若提升成功说明此时对象还活着（且引用计数至少为2），然后可调用其update函数。

对象池
```
class StockFactory : boost::nocopyable {

public:
    shared_ptr<Stock> get(const string& key);

private:
    map<String, shared_ptr<Stock>> stocks;
};
```

此时Stock对象永远不会销毁，因为stocks里面始终保存着一个shared_ptr<Stock>。

若使用weak_ptr好像能解决，但还有个问题。stocks的size()始终还是不变，为其原来的大小。因为当某对象死亡时没有在stocks中删除这个对象对应的元素。

使用另一种方法，在创建对象的shared_ptr时可以指定它的销毁行为，即绑定一个函数：
```
class StockFactory {

// 同上
// 在get()中 将pStock.reset(new Stock(key))改为
// pStock.rest(new Stock(key), boost::bind($StockFactory::deleteStock, this, _1));

void deleteStock(Stock* stock) {
    if(stock) {
        MutexLockGuard lock(m_mutex);
        stocks.erase(stock->key());
    }
    delete stock;
}
};
```
当shared_ptr析构时，会调用deleteStock(ptr), ptr为执行该stock对象指针。
然而这里暴露了StockFac的this指针，若StockFac先于Stock析构，此时去调用就会coredump。因此这里不该传this指针，而是指向当前StockFac对象的shared_ptr

获取指向当前对象的Shread_ptr，需使用shared_from_this(),同时该类要继承 enable_shared_form_this<T>类。
```
class StockFactory : public enable_shared_form_this<StockFactory>
{
public:
    pStock.rest(new Stock(key), boost::bind($StockFactory::deleteStock, shared_form_this(), _1));
};
```
这样可以保证回调时StockFactory对象一定存在。因为function保存了StockFac的一份shared_ptr，延长了其生命周期。

弱回调：有时候我们需要调用时 “如果对象存活，调用其函数，否则忽略”。这时候就应该绑定 weak_ptr对象
```

    // pStock.rest(new Stock(key), boost::bind($StockFactory::deleteStock, weak_ptr<StockFactory>(shared_form_this()), _1));

    void deleteStock(const weak_ptr<Factory>& w_ptr, Stock* stock) {
        Shared_ptr<Factory> ptr(w_ptr.lock());
        if(ptr) {
            ptr->removeStock(stock);
        }
        delete ptr;
    }

```

# 二、线程同步



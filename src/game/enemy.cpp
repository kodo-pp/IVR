#include <modbox/core/core.hpp>
#include <modbox/game/enemy.hpp>
#include <modbox/game/game_loop.hpp>
#include <modbox/geometry/game_position.hpp>
#include <modbox/geometry/geometry.hpp>
#include <modbox/graphics/graphics.hpp>
#include <modbox/modules/module_io.hpp>
#include <modbox/util/util.hpp>

#include <boost/lexical_cast.hpp>
#include <cmath>

Enemy::Enemy(irr::scene::ISceneNode* _node, const std::string& _kind, EnemyId _id)
        : node(_node), kind(_kind), id(_id)
{
    node->grab();
}

Enemy::Enemy(const Enemy& other)
        : node(other.node), kind(other.kind), id(other.id), healthLeft(other.healthLeft), healthMax(other.healthMax), movementSpeed(other.movementSpeed)
{
    node->grab();
}
Enemy::Enemy(Enemy&& other)
        : node(other.node), kind(other.kind), id(other.id), healthLeft(other.healthLeft), healthMax(other.healthMax), movementSpeed(other.movementSpeed)
{
    node->grab();
}
Enemy& Enemy::operator=(const Enemy& other)
{
    Enemy copy(other);
    std::swap(*this, copy);
    return *this;
}
Enemy& Enemy::operator=(Enemy&& other)
{
    std::swap(*this, other);
    return *this;
}

void Enemy::hit(double damage)
{
    healthLeft -= damage;
}

double Enemy::getHealthLeft() const
{
    return healthLeft;
}

void Enemy::setHealthLeft(double health)
{
    healthLeft = health;
}

double Enemy::getHealthMax() const
{
    return healthMax;
}
void Enemy::setHealthMax(double health)
{
    healthMax = health;
}

GamePosition Enemy::getPosition() const
{
    auto position = GamePosition(node->getPosition());
    return position;
}
void Enemy::setPosition(const GamePosition& newPosition)
{
    node->setPosition(newPosition.toIrrVector3df());
}

bool Enemy::isDead() const {
    return healthLeft <= 0;
}

irr::scene::ISceneNode* Enemy::sceneNode() const
{
    return node;
}

void Enemy::ai()
{
    static int counter = 0;
    double fps = /* getFps(); */ 60.0;
    double tickSpeed = /* getTickSpeed(); */ 10.0;
    int updateEach = static_cast<int>(round(fps / tickSpeed));
    try {
    if (counter >= updateEach) {
        counter = 0;
        std::string action = enemyManager.getAiFunction(kind)(id);
        std::vector<std::string> parts;
        boost::algorithm::split(parts, action, [](char c) { return c == ' '; });
        if (parts.size() == 0) {
            goto yes_i_use_goto;
        }
        if (parts.at(0) == "jump") {
            if (parts.size() != 2) {
                throw std::runtime_error("Invalid number of arguments for 'jump' action");
            }
            float jumpSpeed = boost::lexical_cast<float>(parts.at(1));
            graphicsJump(node, jumpSpeed);
        } else if (parts.at(0) == "lookAt") {
            if (parts.size() != 4) {
                throw std::runtime_error("Invalid number of arguments for 'lookAt' action");
            }
            float x = boost::lexical_cast<float>(parts.at(1));
            float y = boost::lexical_cast<float>(parts.at(2));
            float z = boost::lexical_cast<float>(parts.at(3));
            graphicsLookAt(node, x, y, z);
        } else if (parts.at(0) == "setSpeed") {
            if (parts.size() != 2) {
                throw std::runtime_error("Invalid number of arguments for 'setSpeed' action");
            }
            movementSpeed = boost::lexical_cast<double>(parts.at(1));
        } else if (parts.at(0) == "attackPlayer") {
            if (parts.size() != 3) {
                throw std::runtime_error("Invalid number of arguments for 'attackPlayer' action");
            }
            auto maxDistance = boost::lexical_cast<double>(parts.at(1));
            auto damage = boost::lexical_cast<double>(parts.at(2));
            if (
                (
                    getPosition().toIrrVector3df()
                    - getPlayer().getPosition().toIrrVector3df()
                ).getLengthSQ() <= maxDistance * maxDistance
            ) {
                getPlayer().hit(damage);
            }
        } else {
            throw std::runtime_error(std::string("Unknown action: '") + parts.at(0) + "'");
        }
    } else {
        ++counter;
    }
yes_i_use_goto:
    graphicsStep(node, movementSpeed / fps);
    } catch (const std::exception& e) {
        LOG("Exception at Enemy::ai (kind = '" << kind << "')");
        enemyManager.deferredDeleteEnemy(id);
        std::rethrow_exception(std::current_exception());
    }
}

EnemyId EnemyManager::createEnemy(const std::string& kind, irr::scene::ISceneNode* model)
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    ++idCounter;
    enemies.emplace(idCounter, Enemy(model, kind, idCounter));
    creationFunctionsByKind.at(kind)(idCounter);
    return idCounter;
}

void EnemyManager::deleteEnemy(EnemyId id)
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    enemies.erase(id);
}

void EnemyManager::deferredDeleteEnemy(EnemyId id)
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    deferredDeleteQueue.emplace_back(id);
}


const Enemy& EnemyManager::accessEnemy(EnemyId id)
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    return enemies.at(id);
}
Enemy& EnemyManager::mutableAccessEnemy(EnemyId id)
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    return enemies.at(id);
}

EnemyManager enemyManager;

FuncResult handlerAddEnemyKind(const std::vector <std::string>& args)
{
    FuncResult ret;
    if (args.size() != 3) {
        throw std::logic_error("Wrong number of arguments for handlerAddEnemyKind()");
    }

    auto kind = getArgument<std::string>(args, 0);
    auto creationFunc = getArgument<std::string>(args, 1);
    auto aiFunc = getArgument<std::string>(args, 2);

    auto creationFp = getFuncProvider(creationFunc);
    auto aiFp = getFuncProvider(aiFunc);

    enemyManager.addKind(
        kind,
        [=](EnemyId id) { creationFp({std::to_string(id)}); },
        [=](EnemyId id) {
            auto playerPosition = getPlayer().getPosition();
            auto enemyPosition = enemyManager.accessEnemy(id).getPosition();
            return aiFp({
                std::to_string(id),

                std::to_string(enemyPosition.x),
                std::to_string(enemyPosition.y),
                std::to_string(enemyPosition.z),

                std::to_string(playerPosition.x),
                std::to_string(playerPosition.y),
                std::to_string(playerPosition.z)
            }).data.at(0);
        }
    );

    return ret;
}
FuncResult handlerAddEnemy(const std::vector <std::string>& args)
{
    FuncResult ret;
    ret.data.resize(1);
    if (args.size() != 2) {
        throw std::logic_error("Wrong number of arguments for handlerAddEnemy()");
    }

    auto kind = getArgument<std::string>(args, 0);
    auto drawableHandle = getArgument<uint64_t>(args, 1);

    auto enemyId = enemyManager.createEnemy(
        kind,
        drawablesManager.access(drawableHandle)
    );
    
    setReturn(ret, 0, enemyId);
    return ret;
}
FuncResult handlerRemoveEnemy(const std::vector <std::string>& args)
{
    FuncResult ret;
    if (args.size() != 1) {
        throw std::logic_error("Wrong number of arguments for handlerRemoveEnemy()");
    }

    auto enemyId = getArgument<uint64_t>(args, 0);

    enemyManager.deleteEnemy(enemyId);
    
    return ret;
}

void initializeEnemies()
{
    registerFuncProvider(FuncProvider("enemy.addKind", handlerAddEnemyKind), "sss", "");
    registerFuncProvider(FuncProvider("enemy.add", handlerAddEnemy), "su", "u");
    registerFuncProvider(FuncProvider("enemy.remove", handlerRemoveEnemy), "su", "");
}

std::function <std::string(EnemyId)> EnemyManager::getAiFunction(const std::string& kind)
{
    std::lock_guard <std::recursive_mutex> lock(mutex);
    return aiFunctionsByKind.at(kind);
}
void EnemyManager::addKind(
    const std::string& kind,
    const std::function <void(EnemyId)>& creationFunction,
    const std::function <std::string(EnemyId)>& aiFunction
) {
    std::lock_guard <std::recursive_mutex> lock(mutex);
    if (aiFunctionsByKind.count(kind) > 0) {
        throw std::runtime_error("Enemy kind '" + kind + "' already registered");
    }
    aiFunctionsByKind.emplace(kind, aiFunction);
    creationFunctionsByKind.emplace(kind, creationFunction);
}

void EnemyManager::processAi()
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    for (auto& [id, enemy] : enemies) {
        std::ignore = id;
        enemy.ai();
    }
    for (EnemyId enemy : deferredDeleteQueue) {
        deleteEnemy(enemy);
    }
    deferredDeleteQueue.clear();
}

Enemy::~Enemy()
{
    node->drop();
    if (node->getReferenceCount() == 1) {
        node->remove();
    }
}
